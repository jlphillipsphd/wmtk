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

// Forms a complex concept by adding hrrs
HRR HRREngine::addHRRs(vector<string> str_hrrs) {
    vector<HRR> hrrs;
    for (string s : str_hrrs)
        hrrs.push_back(query(s));
    return addHRRs(hrrs);
}
HRR HRREngine::addHRRs(vector<HRR> hrrs) {
    if( hrrs.size() < 1 )
        cerr << "ERROR: no HRR provided\n";

    HRR sum(hrrs[0]);
    for (int i=1; i < hrrs.size(); ++i ) {
        if (hrrs[i].size() != sum.size()){
            cerr << "ERROR: Cannot perform add operation on two vectors of differing size\n";
        }
        for (int j = 0; j < sum.size(); j++) {
            sum[j] += hrrs[i][j];
        }
    }
    /*
    // MJ: I've been getting better results without normalizing the sum vector
    // so I'm leaving this out for now
    for (int j = 0; j < sum.size(); j++) {
        sum[j] /= sqrt(hrrs.size());
    }
    */
    return sum;
}
HRR HRREngine::addHRRs(HRR hrr1,HRR hrr2) {
    HRR sum(hrr1);
    if (hrr2.size() != sum.size()){
        cerr << "ERROR: Cannot perform add operation on two vectors of differing size\n";
    }
    for (int j = 0; j < sum.size(); j++) {
        sum[j] += hrr2[j];
        // MJ: I've been getting better results without normalizing the sum vector
        // so I'm leaving this out for now
        //sum[j] /= sqrt(2);
    }
    return sum;
}

// Forms a complex concept by performing circular convolution on two hrrs
HRR HRREngine::convolveHRRs(vector<string> str_hrrs) {
    vector<HRR> hrrs;
    for (string s : str_hrrs)
        hrrs.push_back(query(s));
    return convolveHRRs(hrrs);
}
HRR HRREngine::convolveHRRs(vector<HRR> hrrs) {
    
    if( hrrs.size() == 0 )
    {
        cerr << "No HRRs passed to convolveHRRs." << endl;
        return HRR();
    }
    // optimization shortcut
    else if( hrrs.size() == 1 )
        return hrrs[0];

    HRR newConcept(hrrs[0]);
    gsl_fft_real_transform(&(newConcept[0]), 1, this->vectorSize, real, work);
    for( int i = 1; i < hrrs.size(); i++ )
    {
        HRR prevConcept = newConcept;
        if (hrrs[i].size() != newConcept.size()) 
        {
            // Notify user if attempted to convolve vectors of differing length
            cout << "ERROR: Cannot convolve two hrrs of differing size!\n";
        } 
        else
        {
          gsl_fft_real_transform(&(hrrs[i][0]), 1, this->vectorSize, real, work);
          multiplycomplex(&(hrrs[i][0]), &(prevConcept[0]), &(newConcept[0]));
        }
    }
    gsl_fft_halfcomplex_inverse(&(newConcept[0]), 1, this->vectorSize, this->hc, this->work);

    return newConcept;
}
HRR HRREngine::convolveHRRs(HRR hrr1, HRR hrr2) {
    HRR newConcept(hrr1.size());

    if (hrr1.size() != hrr2.size()) {
        // Notify user if attempted to convolve vectors of differing length
        cout << "ERROR: Cannot convolve two hrrs of differing size!\n";
    } 
    else {
      gsl_fft_real_transform(&(hrr1[0]), 1, this->vectorSize, real, work);
      gsl_fft_real_transform(&(hrr2[0]), 1, this->vectorSize, real, work);
      multiplycomplex(&(hrr1[0]), &(hrr2[0]), &(newConcept[0]));
      gsl_fft_halfcomplex_inverse(&(newConcept[0]), 1, this->vectorSize, this->hc, this->work);
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

// Used to create simple or complex concepts
// All constituent concepts will be added to memory, as well as the string that was passed in
HRR HRREngine::constructConcept(string concept)
{
    HRR constructedHRR = zero();
    vector<string> disjunct_concepts = explode( concept, '+' );
    for( string d : disjunct_concepts )
    {
        vector<string> conjunct_concepts = explode( d, '*' );
        for( string c : conjunct_concepts )
            if( !hrrInMemory( c ) )
                conceptMemory.insert( pair<string, HRR>( c, generateHRR() ));

        constructedHRR = addHRRs( constructedHRR, convolveHRRs( conjunct_concepts ) );
    }

    if( !hrrInMemory( concept ) )
        conceptMemory.insert(pair<string, HRR>(concept, constructedHRR));

    return constructedHRR;
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
// For performance and consistency, "query" should always be used as the interface to retrieve 
// an HRR representation
HRR HRREngine::query(string name){

    // Reorder the string
    name = reorderNameLex(name);

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

    return constructConcept(name);
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

		// If the dot product is the highest so far, then set the match to be the current representation in the map
		if (dotProduct > threshold && dotProduct > bestMatch){

			// The new best match is the current dot product
			bestMatch = dotProduct;

			// The new match is the name of the current concept
			match = concept.first;
		}
	}
	return match;
}

// Tells whether an HRR has been created or not
bool HRREngine::hrrInMemory(string name)
{
    for (pair<string, HRR> concept: conceptMemory)
        if (concept.first == name)
            return true;
    return false;
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

// Explode a string using a delimiter
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

// Construct an identity vector of given length
HRR HRREngine::zero() {
    return vector<double>(vectorSize, 0.0);
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
