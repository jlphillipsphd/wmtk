//  Program:		Holographic Reduced Representation Engine Implementation
//  Filename:		hrrengine.cpp
//  Author:			Grayson M. Dubois
//  Mentor:			Dr. Joshua L. Phillips

#include <random>
#include <vector>
#include <cmath>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_permute_vector.h>
#include <gsl/gsl_complex.h>
#include <gsl/gsl_complex_math.h>

#include "hrrengine.h"

// MJ: where is threshold set?

using namespace std;

// Default constructor. Sets vector size to 128
HRREngine::HRREngine() : re(1){
    this->vectorSize = 128;
    this->real = gsl_fft_real_wavetable_alloc(this->vectorSize);
    this->hc = gsl_fft_halfcomplex_wavetable_alloc(this->vectorSize);
    this->work = gsl_fft_real_workspace_alloc(this->vectorSize);
        
    this->conceptMemory.insert(pair<string, HRR>("I", identity()));
}

// Initializing Constructor
HRREngine::HRREngine(int vectorSize, int random_seed) : re(random_seed) {
    this->vectorSize = vectorSize;
    this->real = gsl_fft_real_wavetable_alloc(this->vectorSize);
    this->hc = gsl_fft_halfcomplex_wavetable_alloc(this->vectorSize);
    this->work = gsl_fft_real_workspace_alloc(this->vectorSize);
    
    this->conceptMemory.insert(pair<string, HRR>("I", identity()));
}

HRREngine& HRREngine::operator=(const HRREngine& rhs) {

    // Perform a deep copy of the concept memory
    for (pair<string, HRR> concept : rhs.conceptMemory) {
        this->conceptMemory.insert(concept);
    }

    this->vectorSize = rhs.vectorSize;
    this->threshold = rhs.threshold;
    gsl_fft_real_wavetable_free(this->real);
    gsl_fft_halfcomplex_wavetable_free(this->hc);
    gsl_fft_real_workspace_free(this->work);
    this->real = gsl_fft_real_wavetable_alloc(this->vectorSize);
    this->hc = gsl_fft_halfcomplex_wavetable_alloc(this->vectorSize);
    this->work = gsl_fft_real_workspace_alloc(this->vectorSize);

    return *this;
}

HRR HRREngine::generateUnitaryHRR() {
  gsl_complex a;
  size_t i;
  HRR myVec( vectorSize );
  double* halfc = &(myVec[0]);
  //random_device rd;
  //mt19937 e2(rd());
  uniform_real_distribution<double> dist(-M_PI, M_PI);
  
  // Set first element
  halfc[0] = 1.0;
  
  for (i = 1; i < this->vectorSize - i; i++) {      
    a = gsl_complex_polar(1.0, dist(this->re));
    halfc[2 * i - 1] = GSL_REAL(a);
    halfc[2 * i] = GSL_IMAG(a);
  }
  
  if (i == this->vectorSize - i) {
    // Set middle element (just real portion here).
    halfc[this->vectorSize - 1] = 1.0;
  }
  
  gsl_fft_halfcomplex_inverse(halfc, 1, this->vectorSize, hc, work);
  return myVec;
}

// Generates an hrr representation for the given vector
HRR HRREngine::generateHRR() {

  return generateUnitaryHRR();
	// Create a vector of vectorSize
	HRR myVec( vectorSize );

	// Create a random number generator to generate numbers in a gaussian distribution
	//random_device rd;
	//mt19937 e2(rd());

	// Set up the mean and standard deviation
	double mean = 0.0;
	double stdDev = 1.0 / sqrt( vectorSize );

	// Generate a normal distribution to generate numbers from using the mean and stdDev
	normal_distribution<double> dist(mean, stdDev);

	// For each element in the vector, generate a new value from the distribution
	for (double & element : myVec) {
		element = dist(this->re);
	}

	return myVec;
}

// Outputs the contents of an hrr
void HRREngine::printHRRVertical(HRR hrr) {
	for (int i = 0; i < hrr.size(); i++) {
		cout << "| " << setprecision(3) << setw(6) << hrr[i] << " |\n";
	}
}
void HRREngine::printHRRHorizontal(HRR hrr){
	cout << "[ ";
	for (int i = 0; i < hrr.size() - 1; i++){
		cout << setprecision(3) << setw(6) << fixed;
		cout << hrr[i] << " , ";
	}
	cout << setprecision(3) << setw(6) << fixed << hrr[hrr.size() - 1] << setw(3) << " ]";
}

// Combines two concepts to form a complex concept
string HRREngine::combineConcepts(string concept1, string concept2){

	// Create the new name for the object (lexicographical order)
	//	Break the name of the first concept into its constituent parts, and store in descriptors
	//	Then, add each of the constituent parts of the second concept's name into descriptors
	vector<string> descriptors = explode(concept1, '*');
	for (string str : explode(concept2, '*') ){
		descriptors.push_back(str);
	}

	// Sort the descriptors
	sort(descriptors.begin(), descriptors.end());

	// Build the new name for the concept
	string name = "";
	for (int i = 0; i < descriptors.size(); i++) {
		name += descriptors[i];
		if (i != descriptors.size() -1){
			name += "*";
		}
	}

	bool exists = false;
	for ( pair<string, HRR> concept : conceptMemory ) {
		if ( name == concept.first ) exists = true;
	}

	if (!exists){
		// Get the HRRs for each concept
		HRR hrr1 = query(concept1);
		HRR hrr2 = query(concept2);

		// Compute the convolution of the concept
		HRR newHrr = convolveHRRs(hrr1, hrr2);

		// Enter the new concept into concept memory
		conceptMemory.insert(pair<string, HRR>(name, newHrr));
	}

	return name;
}

// Forms a complex concept by performing circular convolution on two hrrs
HRR HRREngine::convolveHRRs(HRR hrr1, HRR hrr2) {
	HRR newConcept(hrr1.size());

	if (hrr1.size() != hrr2.size()) {
		// Notify user if attempted to convolve vectors of differing length
		cout << "ERROR: Cannot convolve two hrrs of differing size!\n";
	} else {


	  gsl_fft_real_transform(&(hrr1[0]), 1, this->vectorSize, real, work);
	  gsl_fft_real_transform(&(hrr2[0]), 1, this->vectorSize, real, work);
	  multiplycomplex(&(hrr1[0]), &(hrr2[0]), &(newConcept[0]));
	  gsl_fft_halfcomplex_inverse(&(newConcept[0]), 1, this->vectorSize, this->hc, this->work);

	  // Form the outer product of hrr1 and hrr2
	// 	vector<vector<float>> outerProduct(hrr1.size());
	// 	for (int i = 0; i < outerProduct.size(); i++){
	// 		vector<float> newVector(hrr2.size());
	// 		outerProduct[i] = newVector;
	// 	}

	// 	// Create the outerProduct matrix
	// 	for (int i = 0; i < hrr1.size(); i ++) {
	// 		for (int j = 0; j < hrr2.size(); j++) {
	// 			outerProduct[i][j] = hrr1[i] * hrr2[j];
	// 		}
	// 	}

	// 	// Perform circular convolution
	// 	for (int j = 0; j < hrr1.size(); j++){
	// 		float sum = 0;
	// 		for (int i = j, k = 0; k <= j; i--, k++){
	// 			sum += hrr1[i] * hrr2[k];

	// 		}
	// 		for (int i = hrr2.size()-1, k = j+1; k <= hrr2.size()-1; i--, k++){
	// 			sum += hrr1[i] * hrr2[k];
	// 		}
	// 		newConcept[j] = sum;
	// 	}

	}

	return newConcept;
}

// Extract a base concept from a complex concept and the former's complementary base concept
string HRREngine::extractConcept(string complexConcept, string baseConcept) {
	string extractedConcept;
	HRR extractedHrr;

	HRR complexHrr = query(complexConcept);
	HRR baseHrr = query(baseConcept);

	extractedHrr = correlateHRRs(complexHrr, baseHrr);

	extractedConcept = query(extractedHrr);

	return extractedConcept;
}

// Perform a circular correlation (involution) operation
HRR HRREngine::correlateHRRs(HRR complexHRR, HRR hrr) {
	HRR newConcept(vectorSize);
	HRR invertedVector(vectorSize);

	// Calculate the approximate inverse of the vector
	invertedVector = invertVector(hrr);

	// Perform involution operation by convolving the complex HRR with the inverted HRR
	newConcept = convolveHRRs(invertedVector, complexHRR);

	return newConcept;
}


// Get user-defined values for an hrr
HRR HRREngine::getUserDefinedHRR(vector<double> values){

	HRR hrr = {};

	// Get values for an HRR for each value
	for (double val : values){
		hrr.push_back(val);
	}

	return hrr;
}

// Method takes a concept name as a string and generates an HRR for it, storing them in concept memory and returning the HRR
HRR HRREngine::encodeConcept(string name){

	for (pair<string, HRR> concept : conceptMemory)
		if ( concept.first == name ) return concept.second;

	// Generate a new HRR for the concept
	HRR newHRR = generateHRR();

	// Add this concept and a new representation for it into the engine
	conceptMemory.insert( pair<string, HRR>( name, newHRR ));

	return newHRR;
}

// Method takes a vector of strings and stores each as an HRR in conceptMemory, if it does not already exist
void HRREngine::encodeConcepts(vector<string> concepts){
	for (string concept : concepts){
		conceptMemory.insert(pair<string, HRR>(concept, generateHRR()));
	}
}

void HRREngine::construct(string conceptName){
	vector<string> concepts = explode(conceptName, '*');
	sort(concepts.begin(), concepts.end());

	constructConcept(concepts);
}

// Method constructs a concept and all of its constituent concepts using a tree-like recursive algorithm
HRR HRREngine::constructConcept(vector<string> concepts){

	// Base case: if there is only one concept, return the representation for that concept
	if (concepts.size() == 1){

		//cout << "\nConcept: " << concepts[0] << "\n";

		// If a representation exists, return it
		return encodeConcept(concepts[0]);
	}
	// Otherwise, iterate over the concepts, and construct a representation for each list of OTHER concepts
	else {

		HRR constructedHRR;

		// Iterate over each concept, constructing a list if it doesn't exist for each combination of OTHER concepts
		for (int i = 0; i < concepts.size(); i++) {

			vector<string> otherVec = concepts;
			otherVec.erase(otherVec.begin() + i);

			string otherConcepts = "";
			for (int i = 0; i < otherVec.size(); i++){
				otherConcepts += ( i < otherVec.size() - 1 ? otherVec[i] + "*" : otherVec[i] );
			}

			string conceptsName = "";
			for (string concept : concepts)
				conceptsName += ( concept == concepts.back() ? concept : concept + "*" );

			if ( findHRRByName(otherConcepts).empty() ){

				constructedHRR = constructConcept(otherVec);

				conceptMemory.insert(pair<string, HRR>(otherConcepts, constructedHRR));
			}

			constructedHRR = convolveHRRs(encodeConcept(concepts[i]), findHRRByName(otherConcepts));

			if (findHRRByName(conceptsName).empty()) conceptMemory.insert(pair<string, HRR>(conceptsName, constructedHRR));
		}

		return constructedHRR;
	}
}

// Returns the general length of the vector
int HRREngine::getVectorSize(){
	return vectorSize;
}

// Sets the general length of the vector
void HRREngine::setVectorSize(int size){
	vectorSize = size;
	gsl_fft_real_wavetable_free(this->real);
	gsl_fft_halfcomplex_wavetable_free(this->hc);
	gsl_fft_real_workspace_free(this->work);
	this->real = gsl_fft_real_wavetable_alloc(this->vectorSize);
	this->hc = gsl_fft_halfcomplex_wavetable_alloc(this->vectorSize);
	this->work = gsl_fft_real_workspace_alloc(this->vectorSize);

    conceptMemory["I"] = identity();
}

/**
 *  Method query() is a critical method for the engine
 *		query() is overloaded to perform two functions, depending on its usage:
 *			- takes a string value as an argument, checks to see if a representation exists for that value,
 *			  and if a representation exists, then returns the HRR for the concept with that name
 *			- takes an HRR as an argument, checks to see if a representation exists for that value, and if
 *			  a representation exists, then returns the name of the concept that matches that representation
 */
HRR HRREngine::query(string name){

	// Reorder the string
	name = reorderNameLex(name);

	vector<string> strings = explode(name, '*');

	// See if a value exists for this concept in the map
	//	For each concept in concept memory, check if the key matches the name of the concept we are looking for
	for (pair<string, HRR> concept : conceptMemory){

		// If the name for the current concept is the name we are looking for, then we have found the concept
		if ( concept.first == name ){

			// Return the hrr for this concept
			return concept.second;
		}
	}

	/**
	 * If we did not return the value, we did not find the concept, thus we need to generate a new HRR for it and add it to the map
	 *	Use construct function to build the concept and all of its constituent pieces
	 */

	HRR newHRR = constructConcept(strings);

	conceptMemory.insert(pair<string, HRR>(name, newHRR));

	return newHRR;
}

string HRREngine::query(HRR hrr){
	float dotProduct;
	float bestMatch = 0;
	string match = "";

	// See if a representation exists for this concept in the map
	//	For each concept in concept memory, compare the HRRs to see if they represent the same concept
	for (pair<string, HRR> concept : conceptMemory){

		// Find the dot product of the queried hrr and the current hrr in the map
		dotProduct = dot(hrr, concept.second);

		//cout << "\nDot product of hrr and " << concept.first << ": " << dotProduct << "\n";

		// If the dot product is the highest so far, then set the match to be the current representation in the map
		if (dotProduct > threshold && dotProduct > bestMatch){

			// The new best match is the current dot product
			bestMatch = dotProduct;

			// The new match is the name of the current concept
			match = concept.first;
		}
        //cout << "End of loop\n";
	}
    //cout << "\n Match: " << match << "\n";
	return match;
}

vector<string> HRREngine::unpack( string complexConcept ){
	vector<string> conceptList;

    //cout << "Concept to unpack: " << complexConcept << "\n";

	unpackRecursive( reorderNameLex(complexConcept), conceptList );
	sort( conceptList.begin(), conceptList.end() );

	return conceptList;
}

void HRREngine::unpackRecursive( string complexConcept, vector<string>& conceptList ){

	complexConcept = reorderNameLex(complexConcept);

    //cout << "Unpacking concept: " << complexConcept << "\n";

	// Get the list of base concepts in the complexConcept
	vector<string> concepts = explode( complexConcept, '*' );

	// Base case: if there is only one concept, return the representation for that concept
	if ( concepts.size() == 1 ) {

        //cout << "Last concept: " << concepts[0] << "\n";

        // Check to see if the concept is in the concept list.
		bool inList = false;
		for ( string conc : conceptList ) {
			if ( conc == concepts[0] ) inList = true;
        }

        // If concept is not in the list, then add it to the list
		if ( !inList ) {
            conceptList.push_back( concepts[0] );
        }

		bool inMemory = false;
		for ( pair<string, HRR> concept : conceptMemory ){
			if ( concept.first == concepts[0] ) inMemory = false;
		}

		if ( !inMemory ) {
			conceptMemory.insert( pair<string, HRR>( concepts[0], encodeConcept(concepts[0]) ) );
		}
	}
	// If there is more than one concept, iterate over the concepts, and construct a
    //  representation for each list of OTHER concepts
	else {

		// Iterate over each concept, constructing a list if it doesn't exist for each
        //  combination of OTHER concepts
		for ( int i = 0; i < concepts.size(); i++ ) {

            // Create a new vector list that contains all the original concepts
			vector<string> otherVec = concepts;

            // Remove the current concept from the list, creating a list of other concepts
			string currentConcept = *(otherVec.begin() + i);
			otherVec.erase( otherVec.begin() + i );

            // Construct a name for the representation of other concepts
			string otherConcepts = "";
			for ( string concept : otherVec ){
				otherConcepts += ( concept == otherVec.back() ? concept : concept + "*" );
			}

            // Call the unpack function recursively on the list of other concepts
			unpackRecursive( otherConcepts, conceptList );

            // Check to see if the original complex concept is in the list
			bool inList = false;
			for ( string conc : conceptList ) {
				if ( conc == complexConcept ) inList = true;
            }

            // If the original complex concept is not in the list, then add it to the list
			if (!inList) {
                conceptList.push_back( complexConcept );
            }

			bool inMemory = false;
			for ( pair<string, HRR> concept : conceptMemory ){
				if ( concept.first == complexConcept ) inMemory = true;
			}

			if ( !inMemory ) {
				conceptMemory.insert( pair<string, HRR>( complexConcept, convolveHRRs( query(currentConcept), query(otherConcepts) ) ) );
			}
		}
	}
}


vector<HRR> HRREngine::unpack(HRR complexConcept){

	vector<HRR> conceptList;
	vector<string> conceptNamesList = unpack( query(complexConcept) );

	for (string s : conceptNamesList){
		conceptList.push_back(findHRRByName(s));
	}

	return conceptList;
}

vector<string> HRREngine::unpackSimple(string complexConcept) {
	return explode(complexConcept, '*');
}

vector<HRR> HRREngine::unpackSimple(HRR complexConcept) {
	vector<HRR> conceptList;
	vector<string> conceptNamesList = explode(query(complexConcept), '*');

	for (auto s : conceptNamesList) {
		conceptList.push_back(findHRRByName(s));
	}

	return conceptList;
}


// Find hrr by name
HRR HRREngine::findHRRByName(string name){
	for (pair<string, HRR> concept: conceptMemory) {
		if (concept.first == name) return concept.second;
    }

    // MJ: having an empty result is a valid case, so don't print an error here
    //cout << "No HRR found for concept: " << name << "\n";
	vector<double> newVector;
	return newVector;
}

// Method lists the map of all concepts. Use only for debugging with small vectors.
void HRREngine::listAllConcepts(){
	// Iterate through the concept map, printing out concepts and their represenations
	for (pair<string, HRR> concept : conceptMemory) {
		cout << concept.first << "\t";
		printHRRHorizontal(concept.second);
		cout << "\n";
	}
}

// Method lists the names of all known concepts.
void HRREngine::listAllConceptNames(){

	// Print the number of concepts in memory
	cout << "Number of Concepts: " << conceptMemory.size() << "\n\n";

	// Iterate throught the concept map, printing out the names of concepts
	for (pair<string, HRR> concept : conceptMemory) {
		cout << concept.first << "\t";
	}
	cout << "\n";
}

// Private helper method invertVector calculates approximate inversion of an HRR
HRR HRREngine::invertVector(HRR hrr) {
	HRR invertedVector(vectorSize);

	invertedVector[0] = hrr[0];

	for (int i = 1; i < vectorSize; i++) {
		invertedVector[i] = hrr[vectorSize - i];
	}

	return invertedVector;
}

// Compare two HRRs by taking their dot product and checking to see if the result is above a threshold
bool HRREngine::compare(HRR hrr1, HRR hrr2){
	float dotProduct = dot(hrr1, hrr2);
	if (dotProduct >= threshold){
		return true;
	}
	return false;
}

// Calculate the dot product of two HRRs
float HRREngine::dot(HRR hrr1, HRR hrr2){

    if (hrr1.size() != hrr2.size()){
        cerr << "ERROR: Cannot perform dot operation on two vectors of differing size\n";
        return 0.0;
    }

	float dotProduct = 0;
	for (int i = 0; i < hrr1.size(); i++) {
		dotProduct += hrr1[i] * hrr2[i];
	}
	return dotProduct;
}

// Explode a string using '*' as a delimiter
vector<string> HRREngine::explode(string str, char delimiter){
	vector<string> stringVector = vector<string>();

	int pos = 0;
	string token;

	while ((pos = str.find(delimiter)) != string::npos){
		token = str.substr(0, pos);
		stringVector.push_back(token);
		str.erase(0, pos + 1);
	}
	stringVector.push_back(str);

	return stringVector;
}

string HRREngine::reorderNameLex(string complexConcept){
	vector<string> conceptNames = explode(complexConcept, '*');
	sort(conceptNames.begin(), conceptNames.end());

	string newName = "";
	for ( int i = 0; i < conceptNames.size() - 1; i++ ) {
		newName += conceptNames[i] + "*";
	}
	newName += conceptNames[conceptNames.size() - 1];
	return newName;
}

// Construct an identity vector of given length
HRR HRREngine::identity() {

    vector<double> identity(vectorSize, 0.0);
    identity[0] = 1.0;

    return identity;
}

void HRREngine::multiplycomplex(double* half1, double* half2, double* result) {
  gsl_complex a,b;
  size_t i;
  
  GSL_SET_REAL(&a,half1[0]);
  GSL_SET_REAL(&b,half2[0]);
  GSL_SET_IMAG(&a,0.0);
  GSL_SET_IMAG(&b,0.0);
  
  result[0] = GSL_REAL(gsl_complex_mul(a,b));
  
  for (i = 1; i < this->vectorSize - i; i++) {
    GSL_SET_REAL(&a,half1[2 * i - 1]);
    GSL_SET_REAL(&b,half2[2 * i - 1]);
    GSL_SET_IMAG(&a,half1[2 * i]);
    GSL_SET_IMAG(&b,half2[2 * i]);
    
    a = gsl_complex_mul(a,b);
    result[2 * i - 1] = GSL_REAL(a);
    result[2 * i] = GSL_IMAG(a);
  }
  
  if (i == this->vectorSize - i) {
    
    GSL_SET_REAL(&a,half1[this->vectorSize - 1]);
    GSL_SET_REAL(&b,half2[this->vectorSize - 1]);
    GSL_SET_IMAG(&a,0.0);
    GSL_SET_IMAG(&b,0.0);
    
    result[this->vectorSize - 1] = GSL_REAL(gsl_complex_mul(a,b));
  }
}
