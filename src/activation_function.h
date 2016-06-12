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
 * activation_function.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef ACTIVATION_FUNCTION_H
#define ACTIVATION_FUNCTION_H

#include <nnet_object.h>

class ActivationFunction : public NNetObject {

 public:
  ActivationFunction();
  ActivationFunction(int number_of_dimensions);

  virtual double Compute(double* variables) = 0;
  virtual double Derivative(int dimension, double* variables) = 0; //partial
  virtual double Derivative(double* variables) = 0; //total

  //Computes the function values relative to the position provided
  double Compute(double* variables, double* position);
  double Derivative(int dimension, double* variables, double* position);
  double Derivative(double* variables, double* position);
  //NOTE: The positional information is assumed to be of the same
  //dimensionality as the input space (number of input dimensions).
  //These functions allow for all derived activation functions to act as
  //radial basis activation functions if desired.

  int getNumberOfInputDimensions();

 protected:
  int dims;

};

#endif
