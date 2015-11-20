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
 * FeatureVector.cpp
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * Basic vector of real values that contains several helper functions for
 * coding values into the vector easily. This is the the class for storing
 * coded representations of a state or chunk.
 *
 * JLP - 5/4/04 9:15PM
 * 		Added constructors, destructor, copy-constructor,
 *              getSize, getValue, and setValue.
 *
 * JLP - 5/5/04 2:35PM
 * 		Added assignment operator. Fixed copy constructor.
 *
 * JLP - 5/5/04
 * 		Fixed assignment operators and copy-constructors
 *
 * JLP - 5/6/04 1:10AM
 * 		Added coarse code functions.
 *
 * JLP - 5/9/04 3:39PM
 * 		Completed "OR" code and added scaling code.
 *
 *****************************************************************************/

#include <FeatureVector.h>
#include <cstddef>
#include <cmath>

#include <iostream>
std::ostream& operator<<(std::ostream& os, const FeatureVector& obj) {
  for (int x = 0; x < obj.getSize(); x++)
    os << obj.getValue(x) << " ";
  return os;
}

FeatureVector::FeatureVector() {
  values = NULL;	// Default to clean array
  size = 0;		// Size of zero for no explicitly provided size
  return;
}

FeatureVector::FeatureVector(int vector_size) {
  if (vector_size < 1) {
    values = NULL;
    size = 0;
    return;			// Clean vector is provided size was incorrect
  }

  size = vector_size;			//Create new vector
  values = new double[size];

  for (int x = 0; x < size; x++) // Initialize values to minimum
    values[x] = WMTK_FV_MIN;

  return;
}

FeatureVector::FeatureVector(const FeatureVector& L) : size(L.size) {
  values = NULL;

  if (size > 1)		// Create new vector
    values = new double[size];

  for (int x = 0; x < size; x++)	// Populate vector values
    values[x] = L.values[x];

  return;
}

FeatureVector& FeatureVector::operator=(const FeatureVector& Rhs) {

  if (this != &Rhs) {

    if (values != NULL)	// Clean old vector
      delete [] values;

    values = NULL;		// Set up for new size
    size = Rhs.size;
    if (size > 0)		// Create new vector
      values = new double[size];

    for (int x = 0; x < size; x++)	// Populate vector values
      values[x] = Rhs.values[x];

  }
  return *this;
}

FeatureVector::~FeatureVector() {
  if (values != NULL)	// If there was a vector, remove it
    delete [] values;

  return;
}

int FeatureVector::getSize() const {
  return size; // return the size
}

double FeatureVector::getValue(int position) const {
  if (position < 0 || position >= size) // Check for valid position
    return -1.0;

  return values[position]; 	// Return the requested value
}

bool FeatureVector::setValue(int position, double value) {
  if (position < 0 || position >= size ||
      value < WMTK_FV_MIN || value > WMTK_FV_MAX) {
    //    std::cout << "Bailed Within... " << position << " " << value << std::endl;
    return false;
  }

  values[position] = value;	// Set the requested value

  return true;
}

void FeatureVector::clearVector() {
  for (int x = 0; x < size; x++)
    values[x] = WMTK_FV_MIN;
  return;
}

bool FeatureVector::thermometerCode(int start_position, int end_position, double minimum, double maximum, double value) {
  if (start_position > end_position || minimum > maximum || start_position < 0 || end_position >= size)
    return false; // Check for out-of-range elements

  int x, y;
  double* threshold = new double[(end_position - start_position) + 1];
  double step = (maximum - minimum) / ((double) (end_position - start_position));
  // Calculate thresholds
  for (x = 0; x < (end_position - start_position) + 1; x++)
    threshold[x] = minimum + (((double) x) * step);

  // Assign vector values
  x = 0;
  for (y = start_position; y <= end_position; y++)
    if (value >= threshold[x++])
      setValue(y, WMTK_FV_MAX);
    else
      setValue(y, WMTK_FV_MIN);

	delete [] threshold;
  return true;
}

bool FeatureVector::gaussianCoarseCode(int start_position, int end_position, double minimum, double maximum, double variance, double value) {
  if (start_position > end_position || minimum > maximum || start_position < 0 || end_position >= size || variance <= 0.0)
    return false; // Check for out-of-range elements

  int x, y;
  double* means = new double[(end_position - start_position) + 1];
  double step = (maximum - minimum) / ((double) (end_position - start_position));
  double scale = (WMTK_FV_MAX - WMTK_FV_MIN);

  // Calculate means
  for (x = 0; x < (end_position - start_position) + 1; x++)
    means[x] = minimum + (((double) x) * step);

  // Assign vector values
  x=0;
  for (y = start_position; y <= end_position; y++) {

    // Calculate gaussian at the position and given mean
    setValue(y, (scale * calculateGaussian(means[x], variance, value)) + WMTK_FV_MIN);
    x++;
  }

	delete [] means;
	return true;
}

bool FeatureVector::speedometerCoarseCode(int start_position, int end_position, double minimum, double maximum, double value) {
  if (start_position > end_position || minimum > maximum || start_position < 0 || end_position >= size)
    return false; // Check for out-of-range elements

  int x, y;
  double* threshold = new double[(end_position - start_position) + 1];
  double step = (maximum - minimum) / ((double) (end_position - start_position) + 1.0);

  // Calculate Thresholds
  for (x = 0; x < (end_position - start_position) + 1; x++)
    threshold[x] = minimum + (((double) x) * step);

  // Assign vector values
  x = 0;
  for (y = start_position; y < end_position; y++) {
    if (value >= threshold[x] && value < threshold[x+1])
      setValue(y, WMTK_FV_MAX);
    else
      setValue(y, WMTK_FV_MIN);
    x++;
  }
  if (value >= threshold[x] && value <= maximum)
    setValue(y, WMTK_FV_MAX);
  else
    setValue(y, WMTK_FV_MIN);

  delete [] threshold;
  return true;
}

bool FeatureVector::makeORCode(const FeatureVector& vector, const OR_CODE type) {
  if (&vector == NULL || vector.getSize() != getSize() || getSize() == 0)
    return false;

  int x;
  double scale = (WMTK_FV_MAX - WMTK_FV_MIN);

  switch (type) {
  case NO_OR:
    return false;
    break;
  case MAX_OR:
    for (x = 0; x < getSize(); x++)
      if (vector.getValue(x) > getValue(x))
	setValue(x, vector.getValue(x));
    break;
  case NOISY_OR:
    for (x = 0; x < getSize(); x++)
      setValue(x, (scale * (1.0 - 
			    ((1.0 - ((getValue(x) - WMTK_FV_MIN) / scale)) *
			     (1.0 - ((vector.getValue(x) - WMTK_FV_MIN) / scale)))
			    )) + WMTK_FV_MIN);
    break;
  }

  return true;
}

double FeatureVector::calculateGaussian(double mean, double variance, double value) {
  // This function is private, so it assumes that all input values have been
  // checked.

  // Originally planned to use actuall gaussian, but it isn't scaled from
  // 0.0 to 1.0. Therefore, the simple calculation below is used. It still
  // allows for a variance-like adjustment, but it is not a true normal
  // distribution. 
  //double sqrt_var = sqrt(variance);
  //double one_over_sqrt_2pi = 1.0 / sqrt(2 * M_PI);
  //double exponent = -((value - mean) * (value - mean)) / (2.0 * variance);
  //double result = (1.0 / sqrt_var) * one_over_sqrt_2pi * exp(exponent);
  double result = exp(-((value - mean) * (value - mean)) / variance);

  return result;
}

int FeatureVector::compare(const FeatureVector& other_vector) const {

  // Size counts -- Longer vecctors are considered larger.
  if (size < other_vector.size)
    return 1;
  else if (size > other_vector.size)
    return -1;

  if (size == 0)
    return 0;

  // Match individual features
  for (int x = 0; x < size; x++) {
    if (values[x] > other_vector.values[x])
      return 1;
    else if (values[x] < other_vector.values[x])
      return -1;
  }

  // Default (they must be equal.)
  return 0;
}
