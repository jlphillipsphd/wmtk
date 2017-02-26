#ifndef HRR_ENGINE
#define HRR_ENGINE

//  Program:		Holographic Reduced Representation Engine Specification
//  Filename:		hrrengine.h
//  Author:		Grayson M. Dubois
//  Mentor:		Dr. Joshua Phillips

#include <map>
#include <vector>
#include <random>

#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>
#include <gsl/gsl_complex.h>

using namespace std;

typedef vector<double> HRR;

class HRREngine {
public:

    /**
     *	Private data members
     */

    // Map stores all concepts as string name and HRR
    map<string, HRR> conceptMemory;

    // Integer indicates the size of vectors used in this engine
    int vectorSize;

    // Double threshold is used to check if the dot product is high enough to consider two concepts equivalent
    double threshold;

    mt19937 re;
    gsl_fft_real_wavetable* real;
    gsl_fft_halfcomplex_wavetable* hc;
    gsl_fft_real_workspace* work;

    /**
     *	Private helper functions
     */

    // Generates an hrr representation for the given vector
    HRR generateHRR();
    HRR generateUnitaryHRR();

    // Helper method invertVector calculates the approximate inversion of an HRR
    HRR invertVector(HRR hrr);

    // Reorder the names of the complex concept in lexicographical order
    string reorderNameLex(string complexConcept);

    // Construct identity vector
    HRR identity();

    // Construct zero vector
    HRR zero();

    void multiplycomplex(double* half1,double* half2,double* result);
  
public:

    /**
     *	Constructors
     */

    // Default constructor
    HRREngine();

    // Initializing Constructor
    HRREngine( int vectorSize, int random_seed = 1 );

    // Copy-Constructor

    // Assignment Operator
    HRREngine& operator=(const HRREngine&);


    /**
     *	Accessors and Mutators
     */

    // Sets the general vectors length
    void setVectorSize(int size);

    /**
     *	Toolkit functions
     */

    // Extract a base concept from a complex concept and the former's complementary base concept
    string extractConcept(string complexConcept, string baseConcept);

    // Output contents of hrr/
    //	It is only recommended to use these methods if you use a low vectorSize
    void printHRRVertical(HRR hrr);
    void printHRRHorizontal(HRR hrr);

    // Get user-defined values for an hrr
    //  Used primarily for testing
    HRR getUserDefinedHRR(vector<double> values);

    // Used to create simple or complex concepts
    // All constituent concepts will be added to memory, as well as the string that was passed in
    HRR constructConcept(string concepts);

    /**
     *  Method query() is a critical method for the engine
     *		query() is overloaded to perform two functions, depending on its usage:
     *			- Takes a string value as an argument, checks to see if a representation exists for that value,
     *			  and if a representation exists, then returns the HRR for the concept with that name.
     *			  If a representation does not exist, constructs one.
     *			- Takes an HRR as an argument, checks to see if a representation exists for that value, and if
     *			  a representation exists, then returns the name of the concept that matches that representation
     */
    // For performance and consistency, "query" should always be used as the inerface to retrieve
    // an HRR representation
    HRR query(string name);
    string query(HRR hrr);

    // Tells whether an HRR has been created or not
    bool hrrInMemory(string name);

    // Method lists the map of all concepts.
    //	It is only recommended to use this method if you use a low vectorSize
    void listAllConcepts();

    // Method lists the names of all known concepts.
    void listAllConceptNames();

    // Forms a complex concept by adding two hrrs
    HRR addHRRs(vector<string> str_hrrs);
    HRR addHRRs(vector<HRR> hrrs);
    HRR addHRRs(HRR hrr1, HRR hrr2);

    // Forms a complex concept by performing circular convolution on two hrrs
    HRR convolveHRRs(vector<string> str_hrrs);
    HRR convolveHRRs(vector<HRR> hrrs);
    HRR convolveHRRs(HRR hrr1, HRR hrr2);

    // Perform a circular correlation (involution) operation
    HRR correlateHRRs(HRR complexHRR, HRR hrr);

    // Compare two HRRs by taking their dot product and checking to see if the result is above a threshold
    bool compare(HRR hrr1, HRR hrr2);

    // Calculate the dot product of two HRRs
    static float dot(HRR hrr1, HRR hrr2);

    // Split a string into a vector of strigs using the given delimiter
    static vector<string> explode(string str, char delimiter);
};

#endif	/* HRR_ENGINE */
