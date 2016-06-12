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
 * AggregateFeatureVector.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class allows for the concatenation of a StateFeatureVector with any
 * number of ChunkFeatureVectors into a single vector representation that can
 * then be provided to the actor and critic networks. It inherits publicly
 * from the FeatureVector class.
 *
 * JLP - 5/5/04 7:13PM
 * 		Initial version created.
 *
 * JLP - 5/6/04 12:45AM
 * 		Added size computation function for constructor use.
 *
 *****************************************************************************/

#ifndef WMTK_AGGREGATE_FEATURE_VECTOR_H
#define WMTK_AGGREGATE_FEATURE_VECTOR_H

#include <FeatureVector.h>

class StateFeatureVector;
class ChunkFeatureVector;

class AggregateFeatureVector : public FeatureVector {

 public:

  // Constructor
  // Creates an empty vector that assumes no chunks for aggregation
  AggregateFeatureVector();

  // Constructor
  // Creates a real vector of the necessary size to handle the
  // concatenation of the number of specified chunks with the specified
  // state vector size.
  AggregateFeatureVector(int state_vector_size,
			 int chunk_vector_size,
			 int number_of_chunks_to_append);

  // Copy-constructor
  AggregateFeatureVector(const AggregateFeatureVector&);

  // Assignment Operator
  AggregateFeatureVector& operator=(const AggregateFeatureVector&);

  // Destructor
  ~AggregateFeatureVector();

  // Maps the feature vectors into the aggregated vector. Returns true
  // if successful, and false otherwise. This is only a copy (and
  // possible combination) of the state and chunk vectors. Thus, the
  // state and chunk vectors should already be appropriately filled with
  // the current values before calling this function. (Call
  // updateFeatures on the state and chunk vectors before passing them
  // to this function.) Pass OR vector also (just assign it all zeros
  // if you are using the NO_OR option to effectively cancel its
  // effects.)
  bool updateFeatures(StateFeatureVector& state_vector,
		      ChunkFeatureVector* chunk_vectors[],
		      FeatureVector& or_vector);

 private:
  int s_vector_size; // The size of the state vector that will be
  // provided.
  int c_vector_size; // The size of the chunk vectors that will be
  // provided.
  int number_of_chunks; // Number of chucks this vector assumes will be
  // provided for concatenation.
  int or_vector_size; // The size of the OR vector - will be the same
		      // as the c_vector_size except for CONJUNCTIVE
		      // codes where c_vector_size = 0
  // Determines the size that an aggregate vector needs to be and checks
  // for inconsistencies in the provided sizes.
  int determineNecessarySize(int state_size,
			     int chunk_size,
			     int num_chunks);
};

#endif
