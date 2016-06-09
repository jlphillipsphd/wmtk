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
 * linear_activation_function.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef LINEAR_ACTIVATION_FUNCTION_H
#define LINEAR_ACTIVATION_FUNCTION_H

#include <activation_function.h>

class LinearActivationFunction : public ActivationFunction {

 private:
  double* gains;
  double offset;

 public:
  LinearActivationFunction();
  ~LinearActivationFunction();
  LinearActivationFunction(int number_of_input_dimensions);
  LinearActivationFunction(int number_of_input_dimensions, double intercept);
  LinearActivationFunction(int number_of_input_dimensions, double intercept, double* slopes);

  double Compute(double* variables);
  double Derivative(int dimension, double* variables);
  double Derivative(double* variables);

  bool setSlope(int dim, double s);
  double getSlope(int dim);
  bool setSlopes(double* s);
  bool getSlopes(double* s);

  bool setIntercept(double val);
  double getIntercept();

};

#endif
