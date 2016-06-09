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
 * layer.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef LAYER_H
#define LAYER_H

#include <nnet_object.h>

class ActivationFunction;
class NeuralUnit;

#ifndef WIN32
using namespace std;
#endif

class Layer : public NNetObject {

 public:
  Layer();
  Layer(const Layer&);
  ~Layer();
  Layer(int number_of_units, ActivationFunction* af);
  Layer& operator=(const Layer&);

  NeuralUnit* getUnit(int index);
  int Size();

  ActivationFunction* getActivationFunction();
  int getNumberOfInputDimensions();

  bool computeActivations();
  bool computePositionalActivations();

  bool applyWTA();										// Uses current activations: off - 0.0, on - 1.0
  bool applyWTA(double off, double on);					// Uses current activations
  bool applySoftWTA();
  bool applySoftWTA(double off);

  // Define error function to use for training the network
  bool useCrossEntropy();
  bool useSSE();

  bool computeBpDeltas();									// Assumes hidden layer
  bool computeBpDeltas(bool use_targets);					// Explicitize for output layer

  // NOTE: Bp deltas here are really just the gradients with
  // respect to the current activation value. This sets all
  // deltas to zero except the specified unit. Thus, it
  // can simply be called for each individual unit.
  // This is only useful to an output layer on either a TD
  // critic layer (TDLayer) or the output layer of a TD
  // actor network.
  bool computeBpDeltas(int unit);

  bool computeBpHumbleDeltas(); 							// Assumes output layer; min 0.0, max 1.0
  bool computeBpHumbleDeltas(double t_min, double t_max);	// Assumes output layer

  bool setActs(double vals[]);
  bool getActs(double vals[]);

  bool setTargets(double vals[]);
  bool getTargets(double vals[]);

  bool setNetDeltas(double vals[]);
  bool getNetDeltas(double vals[]);
  bool clearNetDeltas();

  bool setNets(double* vals[]);
  bool getNets(double* vals[]);
  bool clearNets();

  bool setDeltas(double* vals[]);
  bool getDeltas(double* vals[]);

  bool setDerivatives(double* vals[]);
  bool getDerivatives(double* vals[]);

  bool computeDerivatives();

  double computeSSE();
  // Cross entropy error is only computable for targets and activations
  // that are between 0 and 1 inclusive. Otherwise, this function will
  // return garbage (even if the Bp functions for it still work.)
  double computeCrossEntropy();

  void Print();

 protected:
  int num_units;
  int in_dims;
  double* work_vec2;
  double* work_vec;
  ActivationFunction* act_func;
  bool cross_entropy;

 private:
  NeuralUnit** units;
  void dispose();
  void init();
};

#endif
