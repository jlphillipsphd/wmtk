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

#ifndef WMTK_CRITIC_NETWORK_H
#define WMTK_CRITIC_NETWORK_H

#define GAMMA    0.99
#define LAMBDA   0.0
#define ALPHA    0.0001
#define EPSILON  0.01

#include <fstream>
#include <vector>
#include "hrr/hrrengine.h"

using namespace std;

class CriticNetwork {

  private:

    double alpha;                           // The learning rate of the TD system
    double gamma;                           // The discount values of future states
    double lambda;                          // The eligibility fraction
    double epsilon;

    int vectorSize;                         // Specifies the length of the vectors

  public:

    /**-----------------------------------------------------------------------*
     *  CONSTRUCTORS AND INITIALIZERS
     *------------------------------------------------------------------------*/

    // Default Constructor
    CriticNetwork();

    // Initializing Constructor
    // Creates a critic network with the specified TD values
    CriticNetwork(double newAlpha, double newGamma, double newLambda, double newEpsilon, int newVectorSize);

    // Copy-Constructor
    CriticNetwork(const CriticNetwork&);

    // Destructor
    ~CriticNetwork();

    // Assignment Operator
    CriticNetwork& operator=(const CriticNetwork& criticNetwork);


    /**-----------------------------------------------------------------------*
     *  ACCESSORS
     **-----------------------------------------------------------------------*/

    double getLearningRate();
    double getDiscount();
    double getLambda();
    double getEpsilon();
    int getVectorSize();


    /**-----------------------------------------------------------------------*
     *  MUTATORS
     **-----------------------------------------------------------------------*/

    void setLearningRate(double newLearningRate);
    void setDiscount(double newDiscount);
    void setLambda(double newLambda);
    void setEpsilon(double newEpsilon);
    void setVectorSize(int size);
    void setProperties( double newLearningRate,
                        double newDiscount,
                        double newLambda,
			double newEpsilon,
                        int newVectorSize );


    /**---------------------------------------------------------------------------*
     *  TD LEARNING METHODS
     **---------------------------------------------------------------------------*/

    // Get the values of a given hrr against a weight vector
    double V(HRR hrr, vector<double> weights);

    // Calculate TDError of a state
    double TDError(double r, double valueOfState, double valueOfPreviousState);

    // Calculate TDError of a goal state
    double TDError(double r, double valueOfGoal);
};

#endif      /* WMTK_CRITIC_NETWORK_H */
