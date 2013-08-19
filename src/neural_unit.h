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
 * neural_unit.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef NEURAL_UNIT_H
#define NEURAL_UNIT_H

#include <nnet_object.h>

#ifndef WIN32
using namespace std;
#endif

class NeuralUnit : public NNetObject {

public:
  // Standard Constructor
  NeuralUnit();

  // Copy-Constructor
  NeuralUnit(const NeuralUnit&);

  // Special Constructor
  NeuralUnit(int number_of_input_dimensions);

  // Destructor
  ~NeuralUnit();

  // Assignment Operator
  NeuralUnit& operator=(const NeuralUnit&);

  bool setAct(double val);
  double getAct();

  bool setTarget(double val);
  double getTarget();

  bool setNets(double vals[]);
  bool getNets(double vals[]);

  bool setDerivatives(double vals[]);
  bool getDerivatives(double vals[]);

  bool setNetDelta(double val);
  double getNetDelta();

  bool setDeltas(double vals[]);
  bool getDeltas(double vals[]);

  bool setPositionDimensions(int val);
  int getPositionDimensions();
  double getPosition(int dim);
  bool setPosition(int dim, double val);
  bool getPosition(double vals[]);
  bool setPosition(double vals[]);

  int getInputDimensions();

private:
  double act;
  double target;

  double* nets;
  double net_delta;
  double* deltas;
  double* derivatives;
  int n_dims;

  double* position;
  int p_dims;

  void dispose();
  void init();

};

#endif
