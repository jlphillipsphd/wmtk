// Working Memory Toolkit (WMtk)
// Adaptive Working Memory Library for Robot Control Systems
// Copyright (C) 2005, Joshua L. Phillips & David C. Noelle
// Department of Electrical Engineering and Computer Science
// Vanderbilt University; Nashville, Tennessee, USA.
// Email:  WMtk-Devel@ccnl.vuse.vanderbilt.edu

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

/******************************************************************************
 * CriticNetwork.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class holds all of the details of the critic network which will be
 * used to approximate the value of storing or removing chunks from working
 * memory. It is the workhorse of a TD learning system. It may or may not be
 * used in conjunction with an ActorNetwork to learn how to update the
 * contents of working memory. Using only a critic network will allow faster
 * learning, but it will lead to slower working memory updates (this will
 * probably not be an issue.)
 *
 * JLP - 5/9/04 4:07PM
 * 		Header file created.
 *
 * JLP - 5/21/04 3:36PM
 * 		Added readWeights and writeWeights functions.
 *
 *****************************************************************************/

#ifndef WMTK_CRITIC_NETWORK_H
#define WMTK_CRITIC_NETWORK_H

#define WMTK_CN_GAMMA 0.99
#define WMTK_CN_LAMBDA 0.0
#define WMTK_CN_LEARNING_RATE 0.0001

#include <fstream>

using namespace std;

class FeatureVector;

class Layer;
class TDLayer;
class TDFullForwardProjection;
class SimpleLinearActivationFunction;
class RandomNumberGenerator;

class CriticNetwork {

 public:
	
  // Constructor
  CriticNetwork();

  // Constructor
  // Creates a critic network with an input layer of the specified size.
  CriticNetwork(int input_layer_size);

  // Copy-Constructor
  CriticNetwork(const CriticNetwork&);

  // Destructor
  ~CriticNetwork();

  // Assignment Operator
  CriticNetwork& operator=(const CriticNetwork&);

  // Returns a pointer to the critic_layer. (This is used for creating
  // the actor network, which needs to have access to the critic_layer
  // to process the TD error.)
  TDLayer* getCriticLayer() const;

  // Clears the eligibility traces for the critic network.
  bool clearEligibilityTraces();

  // Initializes the projection with weight values as specified by the
  // RandomNumberGenerator.
  bool initializeWeights(RandomNumberGenerator& rng);

  // Processes the vector representation through the network, then
  // returns the value of the processed vector.
  double processVector(FeatureVector& features);

  // Processes the given vector through the network as the next state.
  // The last time step's representation is the same as that used in the
  // last processVector or processVectorAsNextTimeStep call. The reward
  // information for the current state must be provided as well. Then the
  // function returns the value of the current vector.
  double processVectorAsNextTimeStep(FeatureVector& features,
				     double reward_for_current_time_step,
				     bool learn = true);

  // Uses the last vector processed via processVectorAsNextTimeStep
  // (preferably) as the final state. This case absorbs the provided
  // reward information. The clearEligibilityTraces method should
  // normally be used immediately after calling this method. (However,
  // this choice is left to the network user.)
  double processFinalTimeStep(double reward_for_current_time_step);

  // Writes the weights of the network out to the provided file stream.
  // The stream should be ready to receive the values for writing. The
  // function only returns false if the network is not yet initialized.
  bool writeWeights(ofstream& file_stream);

  // Reads the weights of the network in from the provided file stream.
  // The stream should be ready for reading. The vector sizes of the
  // network weights being used should be the same as this network's
  // vectors. The function only returns false if the network is not yet
  // initialized.
  bool readWeights(ifstream& file_stream);

  // Returns the value of the learning rate being used by the network.
  double getLearningRate() const;

  // Sets the learning rate for the network. Returns false on failure,
  // true otherwise.
  bool setLearningRate(double value);

  // Returns the current reward discount rate (gamma) used by the network.
  double getGamma() const;

  // Sets the reward discount rate (gamma) for the network. Returns false
  // on failure, true otherwise.
  bool setGamma(double value);

  // Returns the eligibility trace discount rate (lambda) used by the
  // network.
  double getLambda() const;

  // Sets the eligibility trace discount rate (lambda) for the network.
  // Returns false on failure, true otherwise.
  bool setLambda(double value);

 private:
  Layer* input_layer; // This is a layer of neural units that will be
  // provided with the representation from an
  // AggregateFeatureVector.
  Layer* bias_layer; // This is a layer containing a single neural unit
  // whose activation will always be 1. Any weights
  // leading from this layer to another will be the
  // bias weights for the receiving units.
  TDLayer* critic_layer; // This is a layer containing a single neural
  // unit which will output the value of the
  // AggregateFeartureVector representation within
  // the input_layer and it also evaluates the
  // change in expected reward (TD error) that
  // occurs between two AggregateFeatureVectors
  // (one at each time step.) 
  TDFullForwardProjection* input_to_critic_projection; // This is a set
  // of connection weights from the input_layer to
  // the critic_layer that fully connects the two
  // layers. These weights will be adjusted according
  // to the TD error computed by the critic_layer.
  TDFullForwardProjection* bias_critic_projection; // This projection
  // contains the bias weight for the critic unit.
  SimpleLinearActivationFunction* slaf; // This is the activation
  // function that will be used by the units in the
  // network.

};

#endif
