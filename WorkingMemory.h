#ifndef WMTK_WORKING_MEMORY_H
#define WMTK_WORKING_MEMORY_H

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
//  MERCHANTIBILITY or FITNESS FOR A PARTICULAT PURPOSE. See the
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
#include <utility>
#include <vector>
#include <random>
#include <math.h>
#include "WorkingMemory.h"
#include "hrr/hrrengine.h"
#include "hrr/hrrOperators.h"
#include "CriticNetwork.h"

using namespace std;

class WorkingMemory {

  public:

    /**-----------------------------------------------------------------------*
     *  DATA MEMBERS & PROPERTIES
     *------------------------------------------------------------------------*/

    HRREngine hrrengine;                        // The HRR Engine handles the representation of information used in working memory
    CriticNetwork critic;                       // The critic determines what states are valuable and thus what chunks are retained in morking memory
    vector<string> workingMemoryChunks;         // The chunks of information currently held in working memory
    string state;                               // The current state
    double currentChunkValue;                   // The current value of the chunks and the state

    double previousReward;                      // Reward of the previous state
    double previousValue;                       // Value of the previous state
    double previousQValue;                      // Q (state/action) value of the previous state

    int vectorSize;                             // The size of the HRRs and other TD vectors
    vector<double> eligibilityTrace;            // Tracks the eligibility of information to update in the weight vector
    vector<double> actionEligibilityTrace;      // Tracks the eligibility of information to update in the weight vector
    vector<double> weights;                     // Stores the weight vector which will be updated to contain the information for the values of each state
    vector<double> actionWeights;               // The weight vector for each action
    double bias;                                // Bias term for the critic neural network

    std::mt19937 re;                            // Random number generator
    vector<int> permutation;                    // The permutation vector used to permute HRRs

  public:

    /**-----------------------------------------------------------------------*
     *  CONSTRUCTORS AND INITIALIZERS
     *------------------------------------------------------------------------*/

    // Default Constructor
    WorkingMemory();

    // Initializing Constructor
    // Initializes Working Memory object with given properties
    WorkingMemory(double learningRate,
                  double discount,
                  double lambda,
                  double epsilon,
                  int vectorSize,
                  double bias,
                  int numberOfChunks,
                  int seed = 1);

    // Copy-Constructor
    WorkingMemory(const WorkingMemory&);

    // Destructor
    ~WorkingMemory();

    // Assignment operator
    WorkingMemory& operator=(const WorkingMemory&);


    /**-----------------------------------------------------------------------*
     *  (*MAIN DEVELOPER INTERFACE*) LEARNING PROCESS METHODS
     *------------------------------------------------------------------------*/

    // Initialize the episode (clears eligibility traces).
    void initializeEpisode();

    // Take a step in the episode.
    //  Takes the string representation of the current state and an optional
    //  value for the reward at that state (typically 0). Calculates a guess of
    //  what information is most valuable to retain from current state.
    string step(string state, vector<string> possibleActions, double reward = 0.0);

    // Get the final reward and finish the episode.
    void absorbReward(double reward = 1.0);

    // Get chunks currently held in working memory
    vector<string> queryWorkingMemory();
    string queryWorkingMemory(int atIndex);

    // Clear the weight vector
    void clearWeights();

    // Reset weight vector
    void resetWeights();
    void resetWeights(double lower, double upper);

  //private:

    /**-----------------------------------------------------------------------*
     *  HELPER METHODS
     *------------------------------------------------------------------------*/

    // Create a weight vector for the action being taken if it doesn't exist
    void createActionWeights(string action);

    // Unpack the state into a vector of possible candidates for working memory
    vector<string> getCandidateChunksFromState();

    // Collects a random selection of candidateChunks to put in working memory
    void chooseRandomWorkingMemoryContents(vector<string> candidates);

    // Compare all possible combinations of candidate chunks to find the most
    // valuable set of working memory contents
    void findMostValuableChunks(vector<string> candidateChunks);

    // Find all combinations of candidate chunks recursively
    void findCombinationsOfCandidates(int offset, int slots, vector<string>& candidates, vector<string>& combination);

    // Find the HRR representing the state
    HRR stateRepresentation();

    // Get the representation of the current working memory contents with the current state
    HRR stateAndWorkingMemoryRepresentation();

    // Calculate the value of the current state
    double findValueOfState();

    // MJ: added this so that we can find the value of the next possible states and pick the best one
    // Calculate the value of a given state using the current working memory contents
    double findValueOfStateWM(vector<string> state);

    // Calculate the value of a given state
    double findValueOfState(vector<string> state);

    // Calculate the value of a given set of working memory contents and current state
    double findValueOfContents(vector<string> contents);

    // This takes a list of possible actions and a given convolutino of state/WM contents
    // and picks the most valuable action to perform
    pair<string,HRR> findMostValuableAction(vector<string> possibleActions);

    // MJ: currently only used for debugging
    // Calculate the value of a given set of working memory contents and a given state
    double findValueOfStateContents(vector<string> state, vector<string> contents);

    // MJ: currently only used for debugging
    // Calculate the value of a given set of working memory contents, a given state, and a given action
    double findValueOfStateContentsAction(vector<string> state, vector<string> contents, string action);

    // MJ - I'm removing it until there's time to clean it up
    /*
    // Set the previous reward and value. Only accessible by WMTK
    void setPreviousStateWorkingMemory(HRR previousStateWM);
    void setPreviousAction(string setPreviousAction);
    void setPreviousReward(double previousReward);
    void setPreviousValue(double previousValue);
    */

    // Perform a permutation on an HRR
    HRR permute(HRR original);

    // Undo the permutation to find the original unshuffled HRR
    HRR inversePermute(HRR permuted);

    // MJ: Debug function
    void printWMContents();    
};

#endif      /* WMTK_WORKING_MEMORY_H */
