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

#include "WorkingMemory.h"

using namespace std;

/**---------------------------------------------------------------------------*
 *  CONSTRUCTORS AND INITIALIZERS
 *----------------------------------------------------------------------------*/

// Default Constructor
WorkingMemory::WorkingMemory() {
    workingMemorySlots = 3;
    vectorSize = 128;

    previousReward = 0.0;
    previousValue = 0.0;

    hrrengine.setVectorSize(vectorSize);
    critic.setVectorSize(vectorSize);

    eligibilityTrace.resize(vectorSize, 0);
    weights.resize(vectorSize, 0);
}

// Initializing Constructor
// Initializes Working Memory object with the given properties
WorkingMemory::WorkingMemory( double learningRate,
                              double discount,
                              double lambda,
                              double epsilon,
                              int vectorSize,
                              int numberOfChunks ) {

    this->workingMemorySlots = numberOfChunks;
    this->vectorSize = vectorSize;

    this->previousReward = 0.0;
    this->previousValue = 0.0;

    this->hrrengine.setVectorSize(vectorSize);
    this->critic.setProperties(learningRate, discount, lambda, vectorSize);

    this->eligibilityTrace.resize(vectorSize, 0);
    this->weights.resize(vectorSize, 0);
}

// Copy-Constructor
WorkingMemory::WorkingMemory(const WorkingMemory& rhs) {

    this->critic = rhs.critic;
    this->hrrengine = rhs.hrrengine;

    this->workingMemoryChunks = rhs.workingMemoryChunks;
    this->workingMemorySlots = rhs.workingMemorySlots;
    this->vectorSize = rhs.vectorSize;

    this->previousReward = rhs.previousReward;
    this->previousValue = rhs.previousValue;

    this->eligibilityTrace = rhs.eligibilityTrace;
    this->weights = rhs.weights;
}

// Assignment Operator
WorkingMemory& WorkingMemory::operator=(const WorkingMemory& rhs) {

    this->critic = rhs.critic;
    this->hrrengine = rhs.hrrengine;

    this->workingMemoryChunks = rhs.workingMemoryChunks;
    this->workingMemorySlots = rhs.workingMemorySlots;
    this->vectorSize = rhs.vectorSize;

    this->previousReward = rhs.previousReward;
    this->previousValue = rhs.previousValue;

    this->eligibilityTrace = rhs.eligibilityTrace;
    this->weights = rhs.weights;

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
