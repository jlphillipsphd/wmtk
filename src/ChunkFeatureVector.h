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
 * ChunkFeatureVector.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class contains an array of real values that represent the information
 * contained in a chunk in real-vector form so that it can be combined with
 * information about the current state. This information can then be evaluated
 * by the critic network and used to make choices about how to maintain
 * working memory via the actor network (or the critic network will do this
 * alone.) This class inherits publicly from the FeatureVector class.
 *
 * JLP - 5/5/04 6:14PM
 *		First draft version.
 *
 *****************************************************************************/

#ifndef WMTK_CHUNK_FEATURE_VECTOR_H
#define WMTK_CHUNK_FEATURE_VECTOR_H

class WorkingMemory;

#include <FeatureVector.h>
#include <Chunk.h>
#include <cstddef>

class ChunkFeatureVector : public FeatureVector {

 public:

  // Constructor
  // Creates an empty vector without an associated function.
  ChunkFeatureVector();

  // Constructor
  // Creates a real vector of the specified size and links it with the
  // provided function.
  ChunkFeatureVector(int vector_size,
		     void (*translation_function)(FeatureVector&,
						  Chunk&,
						  WorkingMemory&));

  // Copy-Constructor
  ChunkFeatureVector(const ChunkFeatureVector&);

  // Assignment Operator
  ChunkFeatureVector& operator=(const ChunkFeatureVector&);
		
  // Destructor
  ~ChunkFeatureVector();

  // Calls the translation function to map the information about the
  // current state into the vector. Returns true if successful, and
  // false otherwise.
  bool	updateFeatures(Chunk& chunk, WorkingMemory& wm);

  // Sets the translation function to the one provided as an argument.
  // Returns true on success and false otherwise.
  bool	setTranslationFunction(void (*translation_function)
			       (FeatureVector&, Chunk&,
				WorkingMemory&));

 private:
  void (*translate)(FeatureVector&, Chunk&, WorkingMemory&);
};

#endif
