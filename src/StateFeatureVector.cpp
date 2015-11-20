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
 * StateFeatureVector.cpp
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class contains an array of real values that represent the current
 * state of the world as provided by the sensory input systems. It inherits
 * publicly from the FeatureVector class to do this.
 *
 * JLP - 5/5/04 - 5:58PM
 * 		First draft version.
 *
 * JLP - 5/6/04 - 1:00AM
 * 		Fixed copy-constructor and assignment operator.
 *
 *****************************************************************************/

#include <StateFeatureVector.h>
#include <cstddef>

StateFeatureVector::StateFeatureVector() : FeatureVector() {
  translate = NULL; // Empty function
  return;
}

StateFeatureVector::StateFeatureVector(int vector_size, void (*translation_function)(FeatureVector&, WorkingMemory&)) : FeatureVector(vector_size) {
  translate = NULL;

  if (translation_function != NULL) // If provided with a function, use it
    translate = translation_function;

  return;
}

StateFeatureVector::StateFeatureVector(const StateFeatureVector& L) : FeatureVector(L) {
  translate = NULL;
  FeatureVector::operator=(L); // Use parent assignment

  if (L.translate != NULL) // If it has a function, use it
    translate = L.translate;

  return;
}

StateFeatureVector& StateFeatureVector::operator=(const StateFeatureVector& Rhs) {
  if (this != &Rhs) {
    FeatureVector::operator=(Rhs);//Use parent assignment

    translate = NULL;

    if (Rhs.translate != NULL) // Again, if it has a function, use it
      translate = Rhs.translate;
  }
  return *this;
}

StateFeatureVector::~StateFeatureVector() {
  translate = NULL; // This may not be needed, but you never know...
}

bool StateFeatureVector::updateFeatures(WorkingMemory& wm) {
  if (translate == NULL)
    return false;	// If we don't have a function, this is a problem

  translate(*this, wm);
  return true;
}

bool StateFeatureVector::setTranslationFunction(void (*translation_function)
						(FeatureVector&,
						 WorkingMemory&)) {

  if (translation_function != NULL) {
    translate = translation_function;
    return true;
  }

  return false;
}
