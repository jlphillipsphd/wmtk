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

#include <fstream>
#include <vector>
#include "hrr/hrrengine.h"
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

    int vectorSize;                             // The size of the hrrs and other TD vectors
    vector<double> eligibilityTrace;            // Tracks the eligibility of information to update in the weight vector
    vector<double> weights;                     // Stores the weight vector which will be updated to contain the information for the values of each state

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
                  int numberOfChunks);

    // Copy-Constructor
    WorkingMemory(const WorkingMemory&);

    // Destructor
    ~WorkingMemory();

    // Assignment operator
    WorkingMemory& operator=(const WorkingMemory&);


    /**-----------------------------------------------------------------------*
     *  ACCESSORS (Getters)
     *------------------------------------------------------------------------*/

    double getLearningRate();
    double getDiscount();
    double getLambda();
    double getEpsilon();
    int getVectorSize();
    int workingMemorySlots();

    double getPreviousReward();
    double getPreviousValue();


    /**-----------------------------------------------------------------------*
     *  MUTATORS (Setters)
     *------------------------------------------------------------------------*/

    void setLearningRate(double newLearningRate);
    void setDiscount(double newDiscount);
    void setLambda(double newLambda);
    void setEpsilon(double newEpsilon);
    void setVectorSize(int newVectorSize);


    /**-----------------------------------------------------------------------*
     *  (*MAIN DEVELOPER INTERFACE*) LEARNING PROCESS METHODS
     *------------------------------------------------------------------------*/

    // Initialize the episode.
    //  Takes the string representation of the initial state and an optional
    //  value for the reward at that state (typically 0). Sets the episode up.
    void initializeEpisode(string state, double reward = 0.0);

    // Take a step in the episode.
    //  Takes the string representation of the current state and an optional
    //  value for the reward at that state (typically 0). Calculates a guess of
    //  what information is most valuable to retain from current state.
    void step(string state, double reward = 0.0);

    // Get the final reward and finish the episode.
    void absorbReward(string state, double reward = 1.0);


    // Get chunks currently held in working memory
    vector<string> queryWorkingMemory();
    string queryWorkingMemory(int atIndex);


  //private:

    /**-----------------------------------------------------------------------*
     *  HELPER METHODS
     *------------------------------------------------------------------------*/

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

    // Calculate the value of a given set of working memory contents and state
    double findValueOfContents(vector<string> contents);

    // Set the previous reward and value. Only accessible by WMTK
    void setPreviousReward(double previousReward);
    void setPreviousValue(double previousValue);

    // Perform a permutation on an HRR
    HRR permute(HRR original);

    // Undo the permutation to find the original unshuffled HRR
    HRR inversePermute(HRR permuted);
};

#endif      /* WMTK_WORKING_MEMORY_H */
