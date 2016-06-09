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
 * StateFeatureVector.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class contains an array of real values that represent the current
 * state of the world as provided by the sensory input systems. It inherits
 * publicly from the FeatureVector class to do this.
 *
 * JLP - 5/4/04 9:04PM
 *		First draft version.
 *
 *****************************************************************************/

#ifndef WMTK_STATE_FEATURE_VECTOR_H
#define WMTK_STATE_FEATURE_VECTOR_H

class WorkingMemory;

#include <FeatureVector.h>

class StateFeatureVector : public FeatureVector {

 public:

  // Constructor
  // Creates an empty vector without an associated function.
  StateFeatureVector();

  // Constructor
  // Creates a real vector of the specified size and links it with the
  // provided function.
  StateFeatureVector(int vector_size,
		     void (*translation_function)(FeatureVector&,
						  WorkingMemory&));

  // Copy-Constructor
  StateFeatureVector(const StateFeatureVector&);

  // Assignment Operator
  StateFeatureVector& operator=(const StateFeatureVector&);
		
  // Destructor
  ~StateFeatureVector();

  // Calls the translation function to map the information about the
  // current state into the vector. Returns true if successful, and
  // false otherwise.
  bool	updateFeatures(WorkingMemory& wm);

  // Sets the translation function to the one provided as an argument.
  // Returns true on success and false otherwise.
  bool    setTranslationFunction(void (*translation_function)
				 (FeatureVector&, WorkingMemory&));

 private:
  void (*translate)(FeatureVector&, WorkingMemory&);
};

#endif
