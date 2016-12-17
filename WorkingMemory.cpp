//  Working Memory Toolkit (WMtk)
//  Adaptive Working Memory Library
//  Copyright (C) Joshua L. Phillips & Grayson M. Dubois
//  Department of Computer Science
//  Middle Tennessee State University; Murfreesboro, Tennessee, USA.

//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.

//  You should have recieved a copy of the GNU General Public License
//  along with this program; if not, write to the Free Sotware
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

/******************************************************************************
 * WorkingMemory.h
 *
 * This software is part of the Working Memory Toolkit.
 *
 * This class holds all of the details of the critic network which will be
 * used to approximate the value of storing or removing chunks from working
 * memory. It is the workhorse of a TD learning system.
 *
 * Author:    Grayson M. Dubois
 * Mentor:    Joshua L. Phillips
 ******************************************************************************/
#include <iostream>
#include <algorithm>
#include <random>
#include "WorkingMemory.h"
#include "hrr/hrrOperators.h"
#include <math.h>

using namespace std;

ostream& operator<<(ostream& os, const vector<string>& strings) {
  int x = 0;
  os << strings[0];
  for (x = 1; x < strings.size(); x++)
    os << "," << strings[x];
  return os;
}

/**---------------------------------------------------------------------------*
 *  CONSTRUCTORS AND INITIALIZERS
 *----------------------------------------------------------------------------*/

int myrandom (int i) { return std::rand()%i;}

// Default Constructor
WorkingMemory::WorkingMemory() {
    workingMemoryChunks.resize(3);

    previousReward = 0.0;
    previousValue = 0.0;

    // Set up the hrr engine and critic network
    vectorSize = 128;
    hrrengine.setVectorSize(vectorSize);
    critic.setVectorSize(vectorSize);

    // Set up the eligibility trace
    eligibilityTrace.resize(vectorSize, 0);

    // Instantiate the weight vector with small random values
    uniform_real_distribution<double> distribution(-0.01, 0.01);
    this->re.seed(1);
    for (int i = 0; i < vectorSize; i++) {
        weights.push_back(distribution(this->re));
    }

    // Set up the random permutation vector
    for ( int i = 0; i < vectorSize; i++ ) {
        permutation.push_back(i);
    }
    random_shuffle(permutation.begin(), permutation.end(), myrandom);
}

// Initializing Constructor
// Initializes Working Memory object with the given properties
WorkingMemory::WorkingMemory( double learningRate,
                              double discount,
                              double lambda,
                              double epsilon,
                              int vectorSize,
                              int numberOfChunks,
							  int seed) {

    //cout << "Started iniitalizing WM\n";

    //cout << "Started iniitalizing WM\n";

    this->workingMemoryChunks.resize(numberOfChunks);

    this->previousReward = 0.0;
    this->previousValue = 0.0;

    // Set up the hrr engine and critic network
    this->vectorSize = vectorSize;
    this->hrrengine.setVectorSize(vectorSize);
    //cout << "HRREngine set up\n";
    this->critic.setProperties(learningRate, discount, lambda, epsilon, vectorSize);
    //cout << "Critic set up\n";

    // Set up the eligibility trace
    this->eligibilityTrace.resize(vectorSize, 0);

    // Instantiate the weight vector with small random values
    uniform_real_distribution<double> distribution(-0.01, 0.01);
    this->re.seed(seed);
    for (int i = 0; i < vectorSize; i++) {
        weights.push_back(distribution(this->re));
    }

    // Set up the random permutation vector
    for ( int i = 0; i < vectorSize; i++ ) {
        permutation.push_back(i);
    }
    random_shuffle(permutation.begin(), permutation.end(), myrandom);
    //cout << "Permutation set up\n";
    return;
}

// Copy-Constructor
WorkingMemory::WorkingMemory(const WorkingMemory& rhs) {

    this->critic = rhs.critic;
    this->hrrengine = rhs.hrrengine;
    this->re = rhs.re;

    for ( string chunk : rhs.workingMemoryChunks ) {
        this->workingMemoryChunks.push_back(chunk);
    }

    this->vectorSize = rhs.vectorSize;

    this->previousReward = rhs.previousReward;
    this->previousValue = rhs.previousValue;

    this->eligibilityTrace.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->eligibilityTrace[i] = rhs.eligibilityTrace[i];
    }

    this->weights.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->weights[i] = rhs.weights[i];
    }

    this->permutation.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->permutation[i] = rhs.permutation[i];
    }
}

// Destructor
WorkingMemory::~WorkingMemory() {}

// Assignment Operator
WorkingMemory& WorkingMemory::operator=(const WorkingMemory& rhs) {

    this->critic = rhs.critic;
    this->hrrengine = rhs.hrrengine;

    for ( string chunk : rhs.workingMemoryChunks ) {
        this->workingMemoryChunks.push_back(chunk);
    }

    this->vectorSize = rhs.vectorSize;

    this->previousReward = rhs.previousReward;
    this->previousValue = rhs.previousValue;

    this->eligibilityTrace.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->eligibilityTrace[i] = rhs.eligibilityTrace[i];
    }

    this->weights.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->weights[i] = rhs.weights[i];
    }

    this->permutation.resize(rhs.vectorSize);
    for ( int i = 0; i < rhs.vectorSize; i++ ) {
        this->permutation[i] = rhs.permutation[i];
    }

    return *this;
}


/**---------------------------------------------------------------------------*
 *  ACCESSORS (Getters)
 *----------------------------------------------------------------------------*/

double WorkingMemory::getLearningRate() { return critic.getLearningRate(); }
double WorkingMemory::getDiscount() { return critic.getDiscount(); }
double WorkingMemory::getLambda() { return critic.getLambda(); }
double WorkingMemory::getEpsilon() { return critic.getEpsilon(); }
int WorkingMemory::getVectorSize() { return vectorSize; }
int WorkingMemory::workingMemorySlots() { return workingMemoryChunks.size(); }

double WorkingMemory::getPreviousReward() { return previousReward; }
double WorkingMemory::getPreviousValue() { return previousValue; }


/**---------------------------------------------------------------------------*
 *  MUTATORS (Setters)
 *----------------------------------------------------------------------------*/

 void WorkingMemory::setLearningRate(double newLearningRate) { this->critic.setLearningRate(newLearningRate); }
 void WorkingMemory::setDiscount(double newDiscount) { this->critic.setDiscount(newDiscount); }
 void WorkingMemory::setLambda(double newLambda) { this->critic.setLambda(newLambda); }
 void WorkingMemory::setEpsilon(double newEpsilon) { this->critic.setEpsilon(newEpsilon); }
 void WorkingMemory::setVectorSize(int newVectorSize) { this->critic.setVectorSize(newVectorSize); }


/**---------------------------------------------------------------------------*
 *  (*MAIN DEVELOPER INTERFACE*) LEARNING PROCESS METHODS
 *----------------------------------------------------------------------------*/

// Initialize the episode.
//  Takes the string representation of the initial state and an optional
//  value for the reward at that state (typically 0). Sets the episode up.
void WorkingMemory::initializeEpisode(string state, double reward) {

  /**
   *  STEP 1: Get Working Memory Contents
   */

    // Store the current state
    this->state = state;

    // Clear working memory
    fill(workingMemoryChunks.begin(),workingMemoryChunks.end(),"I");

    // Get the candidate chunks from the state
    vector<string> candidateChunks = getCandidateChunksFromState();
    sort(candidateChunks.begin(),candidateChunks.end());

    // if (WMdebug) { // Temporary Code Block
    //   cout << "[[" << candidateChunks << "]]" << endl;
    // }

    // Find the most valuable chunks and store in working memory,
    // or random under the Epsilon Soft policy
    uniform_real_distribution<double> distribution(0.0, 1.0);
    if ( distribution(this->re) < getEpsilon() ) {
        chooseRandomWorkingMemoryContents(candidateChunks);
    } else {
        findMostValuableChunks(candidateChunks);
    }

  /**
   *  STEP 2: Store Value and Reward
   */

    // Get the representation of the state and current working memory contents
    HRR representation = stateAndWorkingMemoryRepresentation();

    // Find the value of the current state and working memory contents
    double valueOfState = critic.V(representation, weights);
    //cout << "Initial value: " << valueOfState << "\n";

    if (WMdebug) { // Temporary Code Block
      cout << "Init (" << state << "|" << workingMemoryChunks << ") = "
	   << representation[0] << ":" << valueOfState << " "
	   << "[" << candidateChunks << "]" << endl;
    }

    // Store the value and reward for use in the next step
    setPreviousValue(valueOfState);
    setPreviousReward(reward);

  /**
   *  STEP 3: Clear the Eligibility Trace
   */

    fill(eligibilityTrace.begin(), eligibilityTrace.end(), 0.0);

  /**
   *  STEP 4: Initialize the Eligibility Trace
   */

    //eligibilityTrace = eligibilityTrace + representation;
    for (int x = 0; x < vectorSize; x++) {
        eligibilityTrace[x] += representation[x];
    }
}

// Take a step in the episode.
//  Takes the string representation of the current state and an optional
//  value for the reward at that state (typically 0). Calculates a guess of
//  what information is most valuable to retain from current state.
void WorkingMemory::step(string state, double reward) {

  /**
   *  STEP 1: Get the Working Memory Contents
   */

    // Store the current state
    this->state = state;

    // Build the list of candidate chunks
    vector<string> candidateChunks = getCandidateChunksFromState();

    // Add current working memory contents to the list of candidates, as long as they are not already there
    for ( string chunk : workingMemoryChunks ) {
      if ( chunk != "I" && find( candidateChunks.begin(), candidateChunks.end(), chunk ) == candidateChunks.end() ) {
	candidateChunks.push_back(chunk);
      }
    }
    sort(candidateChunks.begin(),candidateChunks.end());

    // if (WMdebug) { // Temporary Code Block
    //   cout << "[[" << candidateChunks << "]]" << endl;
    // }

    // Find the most valuable chunks and store in working memory,
    // or random under the Epsilon Soft policy
    uniform_real_distribution<double> distribution(0.0, 1.0);
    if ( distribution(this->re) < getEpsilon() ) {
        chooseRandomWorkingMemoryContents(candidateChunks);
    } else {
        findMostValuableChunks(candidateChunks);
    }

  /**
   *  STEP 2: Calculate Value of State and Working Memory Contents
   */

    HRR representation = stateAndWorkingMemoryRepresentation();

    // Find the value of the current state
    double valueOfState = critic.V(representation, weights);
    //cout << "Value of step: " << valueOfState << "\n";

    if (WMdebug) { // Temporary Code Block
    cout << "Step (" << state << "|" << workingMemoryChunks << ") = "
    	 << representation[0] << ":" << valueOfState << " "
    	 << "[" << candidateChunks << "]" << endl;
    }

  /**
   *  STEP 3: Get the TD Error and Update the Weight Vector for the Previous State
   */

    // Find the TDError between the current state and the previous state
    double TDError = critic.TDError(previousReward, valueOfState, previousValue);
    //cout << "TDError of step: " << TDError << "\n";

    // Update the weight vector
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += getLearningRate() * TDError * eligibilityTrace[x];
    }

  /**
   *  STEP 4: Update the Eligibility Trace and Set the Value and Reward
   */

    // Update the eligibility trace of the current state for the next step
    //eligibilityTrace = (eligibilityTrace * getLambda()) + representation;
    for (int x = 0; x < vectorSize; x++) {
        eligibilityTrace[x] *= getLambda();
        eligibilityTrace[x] += representation[x];
    }

    // Store values for next time step
    setPreviousValue(valueOfState);
    setPreviousReward(reward);
}

// Get the final reward and finish the episode.
void WorkingMemory::absorbReward(string state, double reward) {

  /**
   *  STEP 1: Get the Working Memory Contents
   */

    // Store the current state
    this->state = state;

    // Build the list of candidate chunks
    vector<string> candidateChunks = getCandidateChunksFromState();

    // Add current working memory contents to the list of candidates, as long as they are not already there
    for ( string chunk : workingMemoryChunks ) {
      if ( chunk != "I" && find( candidateChunks.begin(), candidateChunks.end(), chunk ) == candidateChunks.end() ) {
	candidateChunks.push_back(chunk);
      }
    }
    sort(candidateChunks.begin(),candidateChunks.end());

    // if (WMdebug) { // Temporary Code Block
    //   cout << "[[" << candidateChunks << "]]" << endl;
    // }

    // Find the most valuable chunks and store in working memory,
    // or random under the Epsilon Soft policy
    uniform_real_distribution<double> distribution(0.0, 1.0);
    if ( distribution(this->re) < getEpsilon() ) {
        chooseRandomWorkingMemoryContents(candidateChunks);
    } else {
        findMostValuableChunks(candidateChunks);
    }

  /**
   *  STEP 2: Calculate Value of State and Working Memory Contents
   */

    HRR representation = stateAndWorkingMemoryRepresentation();

    // Find the value of the current state
    double valueOfState = critic.V(representation, weights);
    //cout << "Value of Absorbing State: " << valueOfState << "\n";

    if (WMdebug) {
      cout << "Absorb (" << state << "|" << workingMemoryChunks << ") = "
	   << representation[0] << ":" << valueOfState << " "
	   << "[" << candidateChunks << "]" << endl;
    }

  /**
   *  STEP 3: Get the TD Error and Update the Weight Vector for the Previous State
   */

    // Find the TDError between the current state and the previous state
    double TDError = critic.TDError(previousReward, valueOfState, previousValue);
    //cout << "TDERROR: " << TDError << "\n";

    // Update the weight vector for the previous state
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += getLearningRate() * TDError * eligibilityTrace[x];
    }

  /**
   *  STEP 4: Calculate Value and Update for Goal
   */

    // Update the eligibility trace for the current goal state
    //eligibilityTrace = (eligibilityTrace * getLambda()) + representation;
    for (int x = 0; x < vectorSize; x++) {
        eligibilityTrace[x] *= getLambda();
        eligibilityTrace[x] += representation[x];
    }

    // Get the value for the goal state
    double TDErrorForGoal = critic.TDError(reward, valueOfState);

    // Update the weight vector for the goal
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += getLearningRate() * TDErrorForGoal * eligibilityTrace[x];
    }

  /**
   *  STEP 5: Reset Eligiblity, and Previous Value and Reward
   */

    // Clear the eligibility trace
    fill(eligibilityTrace.begin(), eligibilityTrace.end(), 0.0);

    // Set the previous value and reward
    setPreviousValue(valueOfState);
    setPreviousReward(0.0);
}

// Get all chunks currently held in working memory
vector<string> WorkingMemory::queryWorkingMemory() {

    vector<string> chunkNames;

    for (string chunk : workingMemoryChunks) {
        chunkNames.push_back(chunk);
    }

    return chunkNames;
}

// Get chunk in working memory at specific index
string WorkingMemory::queryWorkingMemory(int atIndex) {
    return workingMemoryChunks[atIndex];
}


// Clear the weight vector
void WorkingMemory::clearWeights() {
    fill(weights.begin(), weights.end(), 0.0);
}

// Reset the weight vector to small random values between -0.01 and 0.01
void WorkingMemory::resetWeights() {
    uniform_real_distribution<double> distribution(-0.01, 0.01);
    fill(weights.begin(), weights.end(), distribution(this->re));
}

// Reset the weight vector to random values in the specified range
void WorkingMemory::resetWeights(double lower, double upper) {
    uniform_real_distribution<double> distribution(lower, upper);
    fill(weights.begin(), weights.end(), distribution(this->re));
}


/**---------------------------------------------------------------------------*
 *  HELPER METHODS
 *----------------------------------------------------------------------------*/

 // Unpack the state into a vector of possible candidates for working memory
 vector<string> WorkingMemory::getCandidateChunksFromState() {
     vector<string> candidateChunks;

     //cout << "State: " << state << "\n";

     // First, we separate the concepts represented in the state by splitting on
     //  the '+' character
     vector<string> stateConceptNames = HRREngine::explode(state, '+');

     // Now we check if there is an empty string in the list, and remove it
     stateConceptNames.erase( remove( stateConceptNames.begin(), stateConceptNames.end(), "I" ), stateConceptNames.end() );

     /*cout << "State Concept Names: \t";
     for (string name : stateConceptNames) {
         cout << name << "-\t";
     }
     cout << "\n";*/

     // Add the identity vector to the list of candidate chunks
     //for ( int i = 0; i < workingMemorySlots(); i++ ) {
     // candidateChunks.push_back("I");
     //}

     // Add the combinations of constituent concepts for each state concept to
     //  the list of candidate chunks
     vector<string>::iterator iter;
     for (string concept : stateConceptNames) {
         vector<string> candidates = candidateChunks;

         vector<string> unpackedConcepts = hrrengine.unpackSimple(concept);

         candidateChunks.resize( candidates.size() + unpackedConcepts.size());

         // Finds the union of the current candidate chunks with the newly
         //  unpacked concepts, and stores in the candidate chunks vector
         iter = set_union( candidates.begin(), candidates.end(),
                    unpackedConcepts.begin(), unpackedConcepts.end(),
                    candidateChunks.begin() );
     }

     // cout << "Candidate Chunks:\n";
     // for (string chunk : candidateChunks){
     //     cout << chunk << "\n";
     // }

     return candidateChunks;
 }

 // Collects a random selection of candidateChunks to put in working memory
void WorkingMemory::chooseRandomWorkingMemoryContents(vector<string> candidates) {

  // cout << workingMemorySlots() << endl;
  // for (vector<string>::iterator itr = candidates.begin(); itr != candidates.end(); itr++)
  //   cout << *itr << " ";
  // cout << endl;

  for (int i = 0; i < workingMemorySlots(); i++) {
    if (candidates.size() == 0) {
      workingMemoryChunks[i] = "I";
    }
    else {
      uniform_int_distribution<int> distribution(-1,candidates.size()-1);
      int chunk = distribution(this->re);
      if (chunk > 0) {
	workingMemoryChunks[i] = candidates[chunk];
	sort(workingMemoryChunks.begin(),workingMemoryChunks.begin()+i);
	candidates.erase(candidates.begin() + chunk, candidates.begin() + chunk + 1);
      }
      else {
	workingMemoryChunks[i] = "I";
	while (!candidates.empty())
	  candidates.pop_back();
      }
    }
  }

  if (WMdebug) { // Temporary Code block
    HRR representation = stateAndWorkingMemoryRepresentation();
    double valueOfContents = findValueOfContents(workingMemoryChunks);

    cout << "Random (" << state << "|" << workingMemoryChunks << ") = "
	 << representation[0] << ":" << valueOfContents << endl;
  }

}

// Compare all possible combinations of candidate chunks with the
void WorkingMemory::findMostValuableChunks(vector<string> candidateChunks) {

  //cout << "Find Most Valuable Chunks\n";

  // findCombinationsOfCandidates(0, workingMemorySlots(), candidateChunks, mostValuableChunks);

  int n = candidateChunks.size();
  int r = workingMemorySlots();
  if (r > n)
    r = n;

  vector<string> combination(workingMemorySlots());
  fill(combination.begin(),combination.end(),"I");

  workingMemoryChunks = combination;
  currentChunkValue = findValueOfContents(combination);

  if (WMdebug) { // Temporary Code block
    HRR representation = hrrengine.query(combination[0]);
    for ( int i = 1; i < combination.size(); i++ ) {
      representation = hrrengine.convolveHRRs(representation, hrrengine.query(combination[i]));
    }

    // Permute the representation, as long as it is not the identity vector
    if (hrrengine.dot(hrrengine.query("I"),representation) != 1.0)
      representation = permute(representation);

    // Convolve the representation of the WM contents with the state representation
    representation = hrrengine.convolveHRRs(representation,stateRepresentation());

    cout << "(" << state << "|" << combination << ") = "
	 << representation[0] << ":" << currentChunkValue << endl;
  }

  for (int x = 1; x <= r; x++) {
    vector<bool> v(n);
    fill(v.begin(), v.begin() + x, true);

    do {
      int fill = 0;
      for (int i = 0; i < n; ++i) {
	if (v[i]) {
	  combination[fill++] = candidateChunks[i];
	}
      }

      double valueOfContents = findValueOfContents(combination);

      if ( valueOfContents > currentChunkValue ) {
	workingMemoryChunks = combination;
	currentChunkValue = valueOfContents;
      }

      if (WMdebug) { // Temporary Code block
      	HRR representation = hrrengine.query(combination[0]);
      	for ( int i = 1; i < combination.size(); i++ ) {
      	  representation = hrrengine.convolveHRRs(representation, hrrengine.query(combination[i]));
      	}

      	// Permute the representation of
      	if (hrrengine.dot(hrrengine.query("I"),representation) != 1.0)
      	  representation = permute(representation);

      	// Convolve the representation of the WM contents with the state representation
      	representation = hrrengine.convolveHRRs(representation,stateRepresentation());

      	cout << "(" << state << "|" << combination << ") = "
      	     << representation[0] << ":" << valueOfContents << endl;
      }

    } while (prev_permutation(v.begin(), v.end()));
  }

  return;
}

void WorkingMemory::findCombinationsOfCandidates(int offset, int slots, vector<string>& candidates, vector<string>& combination) {

    //cout << "offset: " << offset << "\tslots: " << slots << "\n";


    if (slots == 0) {

        /*for (string chunk : combination) {
            cout << chunk << " | ";
        }
        cout << "\n";*/

        double valueOfContents = findValueOfContents(combination);

        if ( valueOfContents >= currentChunkValue ) {
            workingMemoryChunks = combination;
            currentChunkValue = valueOfContents;
        }

	if (WMdebug) { // Temporary Code block
	  HRR representation = hrrengine.query(combination[0]);
	  for ( int i = 1; i < combination.size(); i++ ) {
	    representation = hrrengine.convolveHRRs(representation, hrrengine.query(combination[i]));
	  }

	  // Permute the representation of
	  if (hrrengine.dot(hrrengine.query("I"),representation) != 1.0)
	    representation = permute(representation);

	  // Convolve the representation of the WM contents with the state representation
	  representation = hrrengine.convolveHRRs(representation,stateRepresentation());

	  cout << "(" << state << "|" << combination << ") = "
	       << representation[0] << ":" << valueOfContents << endl;
	}

        return;
    }

    // Temporary rewrite of below
    for (int i = 0; i < offset+1; i++) {
      combination.push_back(candidates[i]);
      findCombinationsOfCandidates(offset+i, slots-1, candidates, combination);
      combination.pop_back();
    }
    // for (int i = offset; i <= candidates.size() - slots; ++i) {

    //     int begin = i+1;
    //     if (candidates[i] == "I") {
    //         begin = i;
    //     }

    //     combination.push_back(candidates[i]);
    //     findCombinationsOfCandidates(i+1, slots-1, candidates, combination);
    //     combination.pop_back();
    //     if (offset == 0) break;
    // }
}

// Find the HRR representing the state
HRR WorkingMemory::stateRepresentation() {
    vector<string> stateConceptNames = HRREngine::explode(state, '+');
    HRR stateRepresentation = hrrengine.query(stateConceptNames[0]);

    for ( int i = 1; i < stateConceptNames.size(); i++) {
        stateRepresentation = stateRepresentation + hrrengine.query(stateConceptNames[i]);
    }

    return stateRepresentation;
}

// Get the representation of the current working memory contents with the current state
HRR WorkingMemory::stateAndWorkingMemoryRepresentation() {
    // Get the convolved product of each chunk in working memory
    HRR representation = hrrengine.query(workingMemoryChunks[0]);
    for ( int i = 1; i < workingMemorySlots(); i++ ) {
        representation = hrrengine.convolveHRRs(representation, hrrengine.query(workingMemoryChunks[i]));
    }

    // Permute the representation of
    if (hrrengine.dot(hrrengine.query("I"),representation) != 1.0)
      representation = permute(representation);

    // Convolve the representation of the WM contents with the state representation
    representation = hrrengine.convolveHRRs(representation,stateRepresentation());

    return representation;
}

// Calculate the value of the current state
double WorkingMemory::findValueOfState() {
    return critic.V(stateRepresentation(), weights);
}

// Calculate the value of a given set of working memory contents and state
double WorkingMemory::findValueOfContents(vector<string> contents) {

    // Get the convolved product of each chunk in working memory
    HRR representation = hrrengine.query(contents[0]);
    for ( int i = 1; i < contents.size(); i++ ) {
        representation = hrrengine.convolveHRRs(representation, hrrengine.query(contents[i]));
    }

    // Permute the internal representation of working memory contents
    representation = permute(representation);

    // Convolve the representation of the WM contents with the state representation
    representation = hrrengine.convolveHRRs(representation,stateRepresentation());

    // Calculate the value of the representation of the current state and contents
    return critic.V(representation, weights);
}

 // Set the reward of the previous state
void WorkingMemory::setPreviousReward(double previousReward) {
    this->previousReward = previousReward;
}

// Set the value of the previous state
void WorkingMemory::setPreviousValue(double previousValue) {
    this->previousValue = previousValue;
}

// Perform a permutation on an HRR
HRR WorkingMemory::permute(HRR original) {
    HRR permutedHRR(vectorSize);

    for ( int i = 1; i < vectorSize; i++ ) {
        permutedHRR[i] = original[ permutation[i] ];
    }

    return permutedHRR;
}

// Undo the permutation to find the original unshuffled HRR
HRR WorkingMemory::inversePermute(HRR permuted) {
    HRR originalHRR(vectorSize);

    for ( int i = 1; i < vectorSize; i++ ) {
        originalHRR[ permutation[i] ] = permuted[i];
    }

    return originalHRR;
}
