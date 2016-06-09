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
 * linear_activation_function.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <linear_activation_function.h>
#include <cstddef>

LinearActivationFunction::LinearActivationFunction() : ActivationFunction() {
  gains = new double[1];
  gains[0] = 1.0;
  offset = 0.0;
}

LinearActivationFunction::~LinearActivationFunction() {
  delete [] gains;
}

LinearActivationFunction::LinearActivationFunction(int number_of_input_dimensions) : ActivationFunction(number_of_input_dimensions) {
  int x;
  gains = new double[dims];
  for (x = 0; x < dims; x++)
    gains[x] = 1.0;
  offset = 0.0;
}

LinearActivationFunction::LinearActivationFunction(int number_of_input_dimensions, double intercept) : ActivationFunction(number_of_input_dimensions) {

  int x;
  gains = new double[dims];
  for (x = 0; x < dims; x++)
    gains[x] = 1.0;

  offset = intercept;
}

LinearActivationFunction::LinearActivationFunction(int number_of_input_dimensions, double intercept, double* slopes) : ActivationFunction(number_of_input_dimensions) {

  int x;
  gains = new double[dims];
  for (x = 0; x < dims; x++)
    gains[x] = 1.0;

  if (slopes != NULL)
    for (x = 0; x < dims; x++)
      gains[x] = slopes[x];

  offset = intercept;
}


double LinearActivationFunction::Compute(double* variables) {
  if (variables == NULL)
    return 0.0;

  double value = 0.0;

  for (int x = 0; x < dims; x++)
    value += gains[x] * variables[x];

  return (value + offset);
}

double LinearActivationFunction::Derivative(int dimension, double* variables) {
  if (dimension < 0 || dimension >= dims || variables == NULL)
    return 0.0;

  return gains[dimension];
}

double LinearActivationFunction::Derivative(double* variables) {
  if (variables == NULL)
    return 0.0;

  double value = 0.0;

  for (int x = 0; x < dims; x++)
    value += gains[x];

  return value;
}

bool LinearActivationFunction::setSlope(int dim, double s) {
  if (dim < 0 || dim >= dims)
    return false;

  gains[dim] = s;
  return true;
}

double LinearActivationFunction::getSlope(int dim) {
  if (dim < 0 || dim >= dims)
    return 0.0;

  return gains[dim];
}

bool LinearActivationFunction::setSlopes(double* s) {
  if (s == NULL)
    return false;

  for (int x = 0; x < dims; x++)
    gains[x] = s[x];

  return true;
}

bool LinearActivationFunction::getSlopes(double* s) {
  if (s == NULL)
    return false;

  for (int x = 0; x < dims; x++)
    s[x] = gains[x];

  return true;
}

bool LinearActivationFunction::setIntercept(double val) {
  offset = val;
  return true;
}

double LinearActivationFunction::getIntercept() {
  return offset;
}

