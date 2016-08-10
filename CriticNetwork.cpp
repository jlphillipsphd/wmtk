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
 * CriticNetwork.h
 *
 * This software is part of the Working Memory Toolkit.
 *
 * This class holds all of the details of the critic network which will be
 * used to approximate the value of storing or removing chunks from working
 * memory. It is the workhorse of a TD learning system. It may or may not be
 * use in conjunction with an ActorNetwork to learn how to usdate the
 * contents of working memory. Using only a critic network will allow faster
 * learning, but it will lead to slower working memory updates (this will
 * probably not be an issue.)
 *
 * Author:    Grayson M. Dubois
 * Mentor:    Joshua L. Phillips
 ******************************************************************************/

 #include <fstream>
 #include <vector>
 #include <cmath>
 #include "hrr/hrrengine.h"
 #include "hrr/hrrOperators.h"
 #include "CriticNetwork.h"

using namespace std;

/**---------------------------------------------------------------------------*
 *  CONSTRUCTORS AND INITIALIZERS
 *----------------------------------------------------------------------------*/

// Default Constructor
CriticNetwork::CriticNetwork() {
     alpha = ALPHA;
     gamma = GAMMA;
     lambda = LAMBDA;
     epsilon = EPSILON;
     
     vectorSize = 128;
 }

 // Initializing Constructor
 // Creates a critic network with the specified TD values
CriticNetwork::CriticNetwork(double newAlpha, double newGamma, double newLambda, double newEpsilon, int newVectorSize) {
     alpha = newAlpha;
     gamma = newGamma;
     lambda = newLambda;
     epsilon = newEpsilon;

     vectorSize = newVectorSize;
 }

 // Copy-Constructor
CriticNetwork::CriticNetwork(const CriticNetwork& rhs) {
    this->alpha = rhs.alpha;
    this->gamma = rhs.gamma;
    this->lambda = rhs.lambda;
    this->epsilon = rhs.epsilon;

    this->vectorSize = rhs.vectorSize;
}

// Destructor
CriticNetwork::~CriticNetwork() {}

// Assignment Operator
CriticNetwork& CriticNetwork::operator=(const CriticNetwork& rhs) {
    this->alpha = rhs.alpha;
    this->gamma = rhs.gamma;
    this->lambda = rhs.lambda;
    this->epsilon = rhs.epsilon;
    this->vectorSize = rhs.vectorSize;

    return *this;
}


/**---------------------------------------------------------------------------*
 *  ACCESSORS
 **---------------------------------------------------------------------------*/

double CriticNetwork::getLearningRate() { return alpha; }
double CriticNetwork::getDiscount() { return gamma; }
double CriticNetwork::getLambda() { return lambda; }
double CriticNetwork::getEpsilon() { return epsilon; }
int CriticNetwork::getVectorSize() { return vectorSize; }

/**---------------------------------------------------------------------------*
 *  MUTATORS
 **---------------------------------------------------------------------------*/

void CriticNetwork::setLearningRate(double newLearningRate) { this->alpha = newLearningRate; }
void CriticNetwork::setDiscount(double newDiscount) { this->gamma = newDiscount; }
void CriticNetwork::setLambda(double newLambda) { this->lambda = newLambda; }
void CriticNetwork::setEpsilon(double newEpsilon) { this->epsilon = newEpsilon; }
void CriticNetwork::setVectorSize(int newSize) { this->vectorSize = newSize; }

// The setProperties method allows you to set all the properties of the critic
//  in a single call. Useful for initializing if the critic has already been constructed
void CriticNetwork::setProperties( double newLearningRate,
                                   double newDiscount,
                                   double newLambda,
				   double newEpsilon,
                                   int newVectorSize ) {

    this->alpha = newLearningRate;
    this->gamma = newDiscount;
    this->lambda = newLambda;
    this->epsilon = newEpsilon;
    this->vectorSize = newVectorSize;
}

/**---------------------------------------------------------------------------*
 *  TD LEARNING METHODS
 **---------------------------------------------------------------------------*/

// Get the values of a given hrr against a weight vector
double CriticNetwork::V(HRR hrr, vector<double> weights) {
    return HRREngine::dot(hrr, weights);
}

// Calculate TDError of a state
double CriticNetwork::TDError(double r, double valueOfState, double valueOfPreviousState) {
    return (r + gamma*valueOfState) - valueOfPreviousState;
}

// Calculate TDError of a goal state
double CriticNetwork::TDError(double r, double valueOfGoal) {
    return r - valueOfGoal;
}
