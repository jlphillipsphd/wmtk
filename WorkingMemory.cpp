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
#include <random>
#include "WorkingMemory.h"
#include "hrr/hrrOperators.h"
#include <math.h>

using namespace std;

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
    default_random_engine re;
    weights.resize(vectorSize, 0);//distribution(re));

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
                              int numberOfChunks ) {

    this->workingMemoryChunks.resize(numberOfChunks);

    this->previousReward = 0.0;
    this->previousValue = 0.0;

    // Set up the hrr engine and critic network
    this->vectorSize = vectorSize;
    this->hrrengine.setVectorSize(vectorSize);
    this->critic.setProperties(learningRate, discount, lambda, vectorSize);

    // Set up the eligibility trace
    this->eligibilityTrace.resize(vectorSize, 0);

    // Instantiate the weight vector with small random values
    uniform_real_distribution<double> distribution(-0.01, 0.01);
    default_random_engine re;
    this->weights.resize(vectorSize, distribution(re));

    // Set up the random permutation vector
    for ( int i = 0; i < vectorSize; i++ ) {
        permutation.push_back(i);
    }
    random_shuffle(permutation.begin(), permutation.end(), myrandom);
}

// Copy-Constructor
WorkingMemory::WorkingMemory(const WorkingMemory& rhs) {

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

    // Store the current state
    this->state = state;

    // Get the candidate chunks from the state
    vector<string> candidateChunks = getCandidateChunksFromState();

    // Find the most valuable chunks to store in working memory, or random under the epsilon soft policy
    //uniform_real_distribution<double> unif(0.0, 1.0);
    //default_random_engine re;
    //if ( unif(re) < getEpsilon() ) {
    //    chooseRandomWorkingMemoryContents(candidateChunks);
    //} else {
        findMostValuableChunks(candidateChunks);
    //}

    // Find the value of the current state
    double valueOfState = findValueOfWorkingMemoryContents(workingMemoryChunks);

    // Update the eligibility trace
    eligibilityTrace = ( (eligibilityTrace * critic.getLambda()) + stateAndWorkingMemoryRepresentation() ) * (1/sqrt(2));

    // Finish up the initialization
    setPreviousValue(valueOfState);
    setPreviousReward(reward);
}

// Take a step in the episode.
//  Takes the string representation of the current state and an optional
//  value for the reward at that state (typically 0). Calculates a guess of
//  what information is most valuable to retain from current state.
void WorkingMemory::step(string state, double reward) {

    // Store the current state
    this->state = state;

    // Build the list of candidate chunks
    vector<string> candidateChunks = getCandidateChunksFromState();

    // Add current working memory contents to the list of candidates, as long as they are not already there
    for ( string chunk : workingMemoryChunks ) {
        if ( find( candidateChunks.begin(), candidateChunks.end(), chunk ) == candidateChunks.end() ) {
            candidateChunks.push_back(chunk);
        }
    }

    // Find the most valuable chunks to store in working memory, or random under the epsilon soft policy
    //uniform_real_distribution<double> unif(0.0, 1.0);
    //default_random_engine re;
    //if ( unif(re) < getEpsilon() ) {
    //    chooseRandomWorkingMemoryContents(candidateChunks);
    //} else {
        findMostValuableChunks(candidateChunks);
    //}

    // Find the value of the current state
    double valueOfState = findValueOfWorkingMemoryContents(workingMemoryChunks);

    // Find the TDError between the current state and the previous state
    double TDError = critic.TDError(previousReward, valueOfState, previousValue);

    // Update the weight vector
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += critic.getLearningRate() * TDError * eligibilityTrace[x];
    }

    // Update the eligibility trace
    eligibilityTrace = ( (eligibilityTrace * critic.getLambda()) + stateAndWorkingMemoryRepresentation() ) * (1/sqrt(2));

    // Store values for next time step
    setPreviousValue(valueOfState);
    setPreviousReward(reward);
}

// Get the final reward and finish the episode.
void WorkingMemory::absorbReward(string state, double reward) {

    // Store the current state
    this->state = state;

    // Build the list of candidate chunks
        vector<string> candidateChunks = getCandidateChunksFromState();

    // Add current working memory contents to the list of candidates, as long as they are not already there
    for ( string chunk : workingMemoryChunks ) {
        if ( find( candidateChunks.begin(), candidateChunks.end(), chunk ) == candidateChunks.end() ) {
            candidateChunks.push_back(chunk);
        }
    }

    // Find the most valuable chunks to store in working memory, or random under the epsilon soft policy
    //uniform_real_distribution<double> unif(0.0, 1.0);
    //default_random_engine re;
    //if ( unif(re) < getEpsilon() ) {
    //    chooseRandomWorkingMemoryContents(candidateChunks);
    //} else {
        findMostValuableChunks(candidateChunks);
    //}

    // Find the value of the current state
    double valueOfState = findValueOfWorkingMemoryContents(workingMemoryChunks);

    // Find the TDError between the current state and the previous state
    double TDError = critic.TDError(previousReward, valueOfState, previousValue);

    // Update the weight vector
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += critic.getLearningRate() * TDError * eligibilityTrace[x];
    }


    // Update the eligibility trace for the current goal state
    eligibilityTrace = ( (eligibilityTrace * critic.getLambda()) + stateAndWorkingMemoryRepresentation() ) * (1/sqrt(2));

    // Get the value for the goal state
    double TDErrorForGoal = critic.TDError(reward, valueOfState);

    // Update the weight vector for the goal
    for ( int x = 0; x < vectorSize; x++ ) {
        weights[x] += critic.getLearningRate() * TDError * eligibilityTrace[x];
    }
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


/**---------------------------------------------------------------------------*
 *  HELPER METHODS
 *----------------------------------------------------------------------------*/

 // Unpack the state into a vector of possible candidates for working memory
 vector<string> WorkingMemory::getCandidateChunksFromState() {
     vector<string> candidateChunks;

     // First, we separate the concepts represented in the state by splitting on
     //  the '+' character
     vector<string> stateConceptNames = HRREngine::explode(state, '+');

     // Now we check if there is an empty string in the list, and remove it
     stateConceptNames.erase( remove( stateConceptNames.begin(), stateConceptNames.end(), "" ), stateConceptNames.end() );

     // Add the identity vector to the list of candidate chunks
     for ( int i = 0; i < workingMemorySlots(); i++ ) {
        candidateChunks.push_back("I");
     }

     // Add the combinations of constituent concepts for each state concept to
     //  the list of candidate chunks
     vector<string>::iterator iter;
     for (string concept : stateConceptNames) {
         vector<string> candidates = candidateChunks;

         //HRR conceptRepresentation = hrrengine.query(concept);
         //vector<HRR> unpackedConcepts = hrrengine.unpack(conceptRepresentation);

         vector<string> unpackedConcepts = hrrengine.unpack(concept);

         candidateChunks.resize( candidates.size() + unpackedConcepts.size());

         // Finds the union of the current candidate chunks with the newly
         //  unpacked concepts, and stores in the candidate chunks vector
         iter = set_union( candidates.begin(), candidates.end(),
                    unpackedConcepts.begin(), unpackedConcepts.end(),
                    candidateChunks.begin() );
     }

     //candidateChunks.resize(iter - candidateChunks.begin());
     //cout << "Number of candidate chunks: " << candidateChunks.size() << "\n";

     return candidateChunks;
 }

 // Collects a random selection of candidateChunks to put in working memory
void WorkingMemory::chooseRandomWorkingMemoryContents(vector<string> candidates) {
     random_device rd;

     workingMemoryChunks = { "I", "I", "I" };

     for (int i = 0; i < workingMemorySlots(); i++) {
         string chunk;
         do {
             chunk = candidates[rd()%candidates.size()];
         } while ( find( workingMemoryChunks.begin(), workingMemoryChunks.end(), chunk) != workingMemoryChunks.end() );
         workingMemoryChunks[i] = chunk;
     }
 }

// Compare all possible combinations of candidate chunks with the
void WorkingMemory::findMostValuableChunks(vector<string> candidateChunks) {
    vector<string> mostValuableChunks;
    currentChunkValue = -9999999.99;

    findCombinationsOfCandidates(0, workingMemorySlots(), candidateChunks, mostValuableChunks);

    return;
}

void WorkingMemory::findCombinationsOfCandidates(int offset, int slots, vector<string>& candidates, vector<string>& combination) {
    if (slots == 0) {
        /*
        for (string concept : combination) {
            cout << concept << " ";
        }
        cout << "\n";
        */

        // Add the value function and comparison here
        double valueOfContents = findValueOfWorkingMemoryContents(combination);
        //cout << " Value of Combination: " << valueOfContents << "\n";

        if ( valueOfContents > currentChunkValue ) {
            //cout << "Found better WM contents!\n";
            workingMemoryChunks = combination;
            currentChunkValue = valueOfContents;
        }

        return;
    }
    for (int i = offset; i <= candidates.size() - slots; ++i) {
        combination.push_back(candidates[i]);
        findCombinationsOfCandidates(i+1, slots-1, candidates, combination);
        combination.pop_back();
    }
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
    //representation = permute(representation);

    // Convolve the representation of the WM contents with the state representation
    representation = hrrengine.convolveHRRs(representation, stateRepresentation());

    return representation;
}

// Calculate the value of the current state
double WorkingMemory::findValueOfState() {
    return critic.V(stateRepresentation(), weights);
}

// Calculate the value of a given set of working memory contents and state
double WorkingMemory::findValueOfWorkingMemoryContents(vector<string> contents) {

    // Get the convolved product of each chunk in working memory
    HRR representation = hrrengine.query(contents[0]);
    for ( int i = 1; i < contents.size(); i++ ) {
        representation = hrrengine.convolveHRRs(representation, hrrengine.query(contents[i]));
    }

    // Permute the representation of
    representation = permute(representation);

    // Convolve the representation of the WM contents with the state representation
    representation = hrrengine.convolveHRRs(representation, stateRepresentation());

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
