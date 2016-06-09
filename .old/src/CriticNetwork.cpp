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
 * CriticNetwork.cpp
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
 * JLP - 5/9/04 4:35PM
 * 		Source file created.
 *
 * JLP - 5/21/04 4:39PM
 * 		Added neural network processing functions. First full draft.
 *
 *****************************************************************************/

#include <CriticNetwork.h>
#include <cstddef>
#include <fstream>
#include <FeatureVector.h>

// Neural Network Tools
#include <neural_unit.h>
#include <layer.h>
#include <td_layer.h>
#include <td_full_forward_projection.h>
#include <simple_linear_activation_function.h>
#include <random_number_generator.h>


CriticNetwork::CriticNetwork() {
  input_layer = NULL;
  critic_layer = NULL;
  bias_layer = NULL;
  input_to_critic_projection = NULL;
  bias_critic_projection = NULL;
  slaf = NULL;
}

CriticNetwork::CriticNetwork(int input_layer_size) {
  if (input_layer_size < 1) {
    input_layer = NULL;
    critic_layer = NULL;
    bias_layer = NULL;
    input_to_critic_projection = NULL;
    bias_critic_projection = NULL;
    slaf = NULL;
    return;
  }

  slaf = new SimpleLinearActivationFunction();
  bias_layer = new Layer(1, slaf);
  bias_layer->getUnit(0)->setAct(1.0);

  input_layer = new Layer(input_layer_size, slaf);
  critic_layer = new TDLayer(1, slaf, WMTK_CN_GAMMA);

  input_to_critic_projection = new TDFullForwardProjection(input_layer,
							   critic_layer, 0, WMTK_CN_LAMBDA, critic_layer);
  input_to_critic_projection->setLearningRate(WMTK_CN_LEARNING_RATE);
  bias_critic_projection = new TDFullForwardProjection(bias_layer,
						       critic_layer, 0, WMTK_CN_LAMBDA, critic_layer);
  bias_critic_projection->setLearningRate(WMTK_CN_LEARNING_RATE);

}

CriticNetwork::~CriticNetwork() {
  if (input_layer != NULL) {
    delete input_to_critic_projection;
    delete bias_critic_projection;
    delete input_layer;
    delete critic_layer;
    delete bias_layer;
    delete slaf;
  }
}

CriticNetwork::CriticNetwork(const CriticNetwork& L) {
  int x,y;
  input_layer = NULL;
  critic_layer = NULL;
  bias_layer = NULL;
  input_to_critic_projection = NULL;
  bias_critic_projection = NULL;
  slaf = NULL;

  if (L.input_layer != NULL) {
    slaf = new SimpleLinearActivationFunction();
    bias_layer = new Layer(1, slaf);
    bias_layer->getUnit(0)->setAct(1.0);
    input_layer = new Layer(L.input_layer->Size(), slaf);
    critic_layer = new TDLayer(1, slaf, L.critic_layer->getDiscountRate());

    input_to_critic_projection = new TDFullForwardProjection(input_layer,
							     critic_layer, 0, L.input_to_critic_projection->getLambda(),
							     critic_layer);
    input_to_critic_projection->setLearningRate(
						L.input_to_critic_projection->getLearningRate());
    bias_critic_projection = new TDFullForwardProjection(bias_layer,
							 critic_layer, 0, L.bias_critic_projection->getLambda(),
							 critic_layer);
    bias_critic_projection->setLearningRate(
					    L.bias_critic_projection->getLearningRate());

    for (x = 0; x < input_layer->Size(); x++)
      for (y = 0; y < critic_layer->Size(); y++)
	input_to_critic_projection->setWeight(x,y,
					      L.input_to_critic_projection->getWeight(x,y));

    for (y = 0; y < critic_layer->Size(); y++)
      bias_critic_projection->setWeight(0,y,
					L.bias_critic_projection->getWeight(0,y));

  }

}

CriticNetwork& CriticNetwork::operator=(const CriticNetwork& Rhs) {
  int x,y;
  if (this != &Rhs) {
    if (input_layer != NULL) {
      delete input_to_critic_projection;
      delete bias_critic_projection;
      delete input_layer;
      delete critic_layer;
      delete bias_layer;
      delete slaf;
    }

    if (Rhs.input_layer != NULL) {
      slaf = new SimpleLinearActivationFunction();
      bias_layer = new Layer(1, slaf);
      bias_layer->getUnit(0)->setAct(1.0);
      input_layer = new Layer(Rhs.input_layer->Size(), slaf);
      critic_layer = new TDLayer(1, slaf,
				 Rhs.critic_layer->getDiscountRate());

      input_to_critic_projection = new TDFullForwardProjection(
							       input_layer, critic_layer, 0,
							       Rhs.input_to_critic_projection->getLambda(), critic_layer);
      input_to_critic_projection->setLearningRate(
						  Rhs.input_to_critic_projection->getLearningRate());
      bias_critic_projection = new TDFullForwardProjection(bias_layer,
							   critic_layer, 0, Rhs.bias_critic_projection->getLambda(),
							   critic_layer);
      bias_critic_projection->setLearningRate(
					      Rhs.bias_critic_projection->getLearningRate());

      for (x = 0; x < input_layer->Size(); x++)
	for (y = 0; y < critic_layer->Size(); y++)
	  input_to_critic_projection->setWeight(x,y,
						Rhs.input_to_critic_projection->getWeight(x,y));

      for (y = 0; y < critic_layer->Size(); y++)
	bias_critic_projection->setWeight(0,y,
					  Rhs.bias_critic_projection->getWeight(0,y));
    }
  }
  return *this;
}

TDLayer* CriticNetwork::getCriticLayer() const {
  return critic_layer;
}

bool CriticNetwork::clearEligibilityTraces() {
  if (input_layer == NULL)
    return false;

  return input_to_critic_projection->clearEligibilityTraces() &&
    bias_critic_projection->clearEligibilityTraces();
}

bool CriticNetwork::initializeWeights(RandomNumberGenerator& rng) {
  if (input_layer == NULL)
    return false;

  return input_to_critic_projection->initializeWeights(&rng) && 
    bias_critic_projection->initializeWeights(&rng);
	
}

double CriticNetwork::processVector(FeatureVector& features) {
  if (&features == NULL || input_layer == NULL || features.getSize() != input_layer->Size())
    return 0.0;

  int x;

  for (x = 0; x < features.getSize(); x++) {
    input_layer->getUnit(x)->setAct(features.getValue(x));
  }

  critic_layer->clearNets();
  input_to_critic_projection->adjustNets();
  bias_critic_projection->adjustNets();
  critic_layer->computeActivations();

  return critic_layer->getUnit(0)->getAct();

}

double CriticNetwork::processFinalTimeStep(double reward_for_current_time_step) {
  double reward[1];
  reward[0] = reward_for_current_time_step;

  critic_layer->computeBpDeltas(0);
  input_to_critic_projection->updateEligibilityTraces(0);
  bias_critic_projection->updateEligibilityTraces(0);
  critic_layer->shiftActivationState();

  critic_layer->computeTDError(reward, true);
  input_to_critic_projection->computeWeightChanges(0);
  bias_critic_projection->computeWeightChanges(0);
  input_to_critic_projection->updateWeights();
  bias_critic_projection->updateWeights();

  critic_layer->clearNets();
  input_to_critic_projection->adjustNets();
  bias_critic_projection->adjustNets();
  critic_layer->computeActivations();

  return critic_layer->getUnit(0)->getAct();
}

double CriticNetwork::processVectorAsNextTimeStep(FeatureVector& features, double reward_for_current_time_step, bool learn) {
  if (&features == NULL || input_layer == NULL || features.getSize() != input_layer->Size())
    return 0.0;

  double reward[1];
  reward[0] = reward_for_current_time_step;

  if (learn) {
    critic_layer->computeBpDeltas(0);
    input_to_critic_projection->updateEligibilityTraces(0);
    bias_critic_projection->updateEligibilityTraces(0);
    critic_layer->shiftActivationState();
    
    processVector(features);
    
    critic_layer->computeTDError(reward);
    input_to_critic_projection->computeWeightChanges(0);
    bias_critic_projection->computeWeightChanges(0);
    input_to_critic_projection->updateWeights();
    bias_critic_projection->updateWeights();
  }

  return processVector(features);

}

bool CriticNetwork::writeWeights(ofstream& file_stream) {
  if (input_layer == NULL)
    return false;

  return
    input_to_critic_projection->writeWeights(file_stream) &&
    bias_critic_projection->writeWeights(file_stream);
}

bool CriticNetwork::readWeights(ifstream& file_stream) {
  if (input_layer == NULL)
    return false;

  return
    input_to_critic_projection->readWeights(file_stream) &&
    bias_critic_projection->readWeights(file_stream);
}

bool CriticNetwork::setLearningRate(double value) {
  if (input_to_critic_projection != NULL && bias_critic_projection != NULL)
    {
      return input_to_critic_projection->setLearningRate(value) &&
	bias_critic_projection->setLearningRate(value);
    }
  return false;
}

double CriticNetwork::getLearningRate() const {
  if (input_to_critic_projection != NULL) {
    return input_to_critic_projection->getLearningRate();
  }
  return 0.0;
}

bool CriticNetwork::setGamma(double value) {
  if (critic_layer != NULL) {
    return critic_layer->setDiscountRate(value);
  }
  return false;
}

double CriticNetwork::getGamma() const {
  if (critic_layer != NULL) {
    return critic_layer->getDiscountRate();
  }
  return -1.0;
}

bool CriticNetwork::setLambda(double value) {
  if (input_to_critic_projection != NULL && bias_critic_projection != NULL)
    {
      return input_to_critic_projection->setLambda(value) &&
	bias_critic_projection->setLambda(value);
    }
  return false;
}

double CriticNetwork::getLambda() const {
  if (input_to_critic_projection != NULL) {
    return input_to_critic_projection->getLambda();
  }
  return -1.0;
}
