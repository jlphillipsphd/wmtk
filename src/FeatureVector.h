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
 * FeatureVector.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * Basic vector of real values that contains several helper functions for
 * coding values into the vector easily. This is the the class for storing
 * coded representations of a state or chunk.
 *
 * JLP - 5/4/04 8:48PM
 * 		First complete version.
 *
 * JLP - 5/6/04 12:30AM
 * 		Added gaussian calculation function.
 *
 * JLP - 5/7/04 1:33PM
 * 		Added coarse code functions
 *
 * JLP - 5/9/04 3:08PM
 * 		Added makeORCode function and enumeration.
 *
 *****************************************************************************/

#ifndef WMTK_FEATURE_VECTOR_H
#define WMTK_FEATURE_VECTOR_H

#include <iostream>

// Defines the maximum and minimum values that can be stored in the feature
// vector elements. Incorrect values here will lead to problems. These values
// are used to check for proper assignment as well as scaling on the coarse
// coding functions.
#define WMTK_FV_MIN 0.0
#define WMTK_FV_MAX 1.0

// Enumeration of "OR" codes.
// Each code is used by the makeORCode function to decide how to compute the
// "OR" of two vectors. The NO_OR flag simply ignores an attempt to "OR" the
// vector. The MAX_OR flag generates an "OR" of the vectors that contains the
// maximum between corresponding vector values. The NOISY_OR flag generates
// the "OR" of two vectors which uses a fuzzy logical interpretation of the
// vectors.
enum OR_CODE {
  NO_OR,
  MAX_OR,
  NOISY_OR
};

class FeatureVector {

 public:

  // Default Constructor
  // Creates an empty vector.
  FeatureVector();

  // Constructor
  // Creates a real vector of the specified size.
  FeatureVector(int vector_size);

  // Copy-Constructor
  FeatureVector(const FeatureVector&);

  // Assignment Operator
  FeatureVector& operator=(const FeatureVector&);

  // Destructor
  ~FeatureVector();

  // Returns zero if the elements of the provided feature vector match
  // this vector, negative if the provided vector is less than this
  // vector, and positive if the provided vector is greater than this
  // vector. (This comparison is only used  for canonical ordering of
  // feature vectors and has no mathematical relevance.)
  int compare(const FeatureVector&) const;

  int getSize() const; // Returns the size of the vector.

  // Returns the element of the vector in the specified position
  // (indexed by zero).
  double	getValue(int position) const;

  // Sets the vector element at the specified position (indexed by
  // zero).
  bool	setValue(int position, double value);

  // Sets the vector elements to their minimum values.
  void	clearVector();

  // This function encodes the value provided in thermometer code within
  // the specified range of vector positions (inclusive) while scaling
  // according to the minimum and maximum provided. The final individual
  // vector values are in the range (0-1) (indexed by zero).
  bool	thermometerCode(int start_position,
			int end_position,
			double minimum,
			double maximum,
			double value);

  // This function encodes the value provided using a gaussian coarse
  // coding of the specified variance within the specified range of
  // vector positions (inclusive) while scaling according to the minimum
  // and maximum provided. The final individual vector values are in the
  // range (0-1] (indexed by zero).
  bool	gaussianCoarseCode(int start_position,
			   int end_position,
			   double minimum,
			   double maximum,
			   double variance,
			   double value);

  // This function encodes the value provided in standard threshold
  // coarse coding within the specified range of vector positions
  // (inclusive) while scaling according to the minimum and maximum
  // provided. The final vector values are either 0 or 1. (indexed by
  // zero).
  bool	speedometerCoarseCode(int start_position,
			      int end_position,
			      double minimum,
			      double maximum,
			      double value);
								

  // Sets this vector to the “OR” of this feature vector with the
  // provided feature vector. (The vectors must be the same size for
  // this to succeed.)
  bool	makeORCode(const FeatureVector& vector, const OR_CODE type);

  friend std::ostream& operator<<(std::ostream& os, const FeatureVector& obj);

 private:
  double* values; // Vector of real values for coding values.
  int size; // Size of the vector.

  // This function computes the value of a gaussian function with the
  // provided mean and variance.
  double calculateGaussian(double mean,
			   double variance,
			   double value);

};

#endif
