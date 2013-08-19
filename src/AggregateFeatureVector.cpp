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
 * AggregateFeatureVector.cpp
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class allows for the concatenation of a StateFeatureVector with any
 * number of ChunkFeatureVectors into a single vector representation that can
 * then be provided to the actor and critic networks. It inherits publicly
 * from the FeatureVector class.
 *
 * Joshua Phillips - 5/5/04 7:24PM
 * 		Initial version created.
 *
 * Joshua Phillips - 5/6/04 1:14AM
 * 		Fixed initialization problems with zero size state or
 *              zero chunks.
 *
 * Joshua Phillips - 7/16/04 2:31PM
 *		Added some features for testing possible conjunctive
 *              coding of state and chunk vectors. These are commented
 *              out, but will be retained until future examination
 *              deems them absolutely unnecessary.
 *
 * Joshua Phillips - Vanderbilt University - Sun Mar 19 23:47:03 2006
 *              Simplified coding scheme to include OR code as always
 *              being concatenated with all other codes, conjunctive
 *              or otherwise.
 *
 *****************************************************************************/

#include <AggregateFeatureVector.h>
#include <StateFeatureVector.h>
#include <ChunkFeatureVector.h>
#include <stddef.h>
#include <iostream>
#include <math.h>

enum coding_scheme {
  CONCAT,
  WM_CONJ,
  COMPLETE_CONJ,
  WM_STATE_CONJ
};

static coding_scheme afv_code = WM_CONJ;

using namespace std;

//#define WMTK_DEBUG
#define USER_BASED_CONJUNCTION

AggregateFeatureVector::AggregateFeatureVector() : FeatureVector() {
  s_vector_size = 0;	// Initial values for empty vector
  c_vector_size = 0;
  number_of_chunks = 0;
  or_vector_size = 0;
  return;
}

AggregateFeatureVector::AggregateFeatureVector(int state_vector_size, int chunk_vector_size, int number_of_chunks_to_append) : FeatureVector(determineNecessarySize(state_vector_size, chunk_vector_size, number_of_chunks_to_append)) {
  s_vector_size = 0; // Initial value for empty vector
  c_vector_size = 0;
  number_of_chunks = 0;
  or_vector_size = 0;

  // We defer error checking to the determineNecessarySize function. If it
  // determines that the arguments were OK, then the size of the vector will
  // be positive (it will make a correctly sized vector). Otherwise, the
  // function will pass back a zero, essentially creating a zero-sized
  // vector and we can check for this.
  if (getSize() > 0) {
    s_vector_size = state_vector_size;
    c_vector_size = chunk_vector_size;
    number_of_chunks = number_of_chunks_to_append;
    or_vector_size = chunk_vector_size;

    if (c_vector_size == 0 || number_of_chunks == 0) {
      c_vector_size = 0;
      number_of_chunks = 0;
    }
  }

  return;
}

AggregateFeatureVector::AggregateFeatureVector(const AggregateFeatureVector& L) : FeatureVector(L) {
  FeatureVector::operator=(L); // Use parent assignment

  s_vector_size = L.s_vector_size;
  c_vector_size = L.c_vector_size;
  number_of_chunks = L.number_of_chunks;
  or_vector_size = L.or_vector_size;
  return;	
}

AggregateFeatureVector& AggregateFeatureVector::operator=(const AggregateFeatureVector& Rhs) {
  if (this != &Rhs) {
    FeatureVector::operator=(Rhs); // Use parent assignment

    s_vector_size = Rhs.s_vector_size;
    c_vector_size = Rhs.c_vector_size;
    number_of_chunks = Rhs.number_of_chunks;
    or_vector_size = Rhs.or_vector_size;
  }
  return *this;
}

AggregateFeatureVector::~AggregateFeatureVector() {
  s_vector_size = 0;	// This function may not be necessary (or wanted).
  c_vector_size = 0;
  number_of_chunks = 0;
  or_vector_size = 0;
}

bool AggregateFeatureVector::updateFeatures(StateFeatureVector& state_vector, ChunkFeatureVector* chunk_vectors[],  FeatureVector& or_vector) {
  int x, y, z; // Counters
  int s;					// Variables for Full Conjunctive Coding
  int* c = new int[number_of_chunks];

  bool success = true; // Assume we will succeed

  // Basic check
  if (getSize() == 0) {
	delete [] c;
	return false;
  }
  
  // Check for vectors of the proper size
  if (&state_vector == NULL ||
      (state_vector.getSize() != s_vector_size &&
       c_vector_size == 0) || (chunk_vectors == NULL && s_vector_size == 0)
      || (or_vector.getSize() != or_vector_size)) {
	delete [] c;
	return false;
  }

  // Check chunk_vector sizes
  for (x = 0; x < number_of_chunks; x++)
    if (chunk_vectors[x]->getSize() != c_vector_size) {
      delete [] c;
      return false;
    }

#ifdef WMTK_DEBUG
  cout << "Aggregate Sizes Checked: CLEAN" << endl;
  cout << "State Vector: " << state_vector << endl;
  cout << "Chunk Vectors:" << endl;
  for (x = 0; x < number_of_chunks; x++)
    if (chunk_vectors[x] == NULL)
      cout << "NULL" << endl;
    else
      cout << *(chunk_vectors[x]) << endl;
  cout << "OR Vector: " << or_vector << endl;
#endif

  // DEBUG - Output state and chunk vectors to check encodings.
  /*
    cout << "State Vector: ";
    for (x = 0; x < state_vector.getSize(); x++)
    cout << state_vector.getValue(x) << " ";
    cout << endl;
	
    for (x = 0; x < number_of_chunks; x++) {
    cout << "Chunk Vector #" << (x+1) << ": ";
    for (y = 0; y < chunk_vectors[x]->getSize(); y++)
    cout << chunk_vectors[x]->getValue(y) << " ";
    cout << endl;
    }
  */

  // Make conjunctive code...

  if (afv_code == WM_STATE_CONJ) {

    if (number_of_chunks > 0) {
      // Conjunct Working Memory and State, but not individual chunks
      FeatureVector chunks(c_vector_size * number_of_chunks);
      for (x = 0; x < number_of_chunks; x++)
	for (y = 0; y < c_vector_size; y++)
	  chunks.setValue((x*c_vector_size) + y, chunk_vectors[x]->getValue(y));
      
      FeatureVector state_chunks(s_vector_size * chunks.getSize());
      double value;
      double value2;
      for (x = 0; x < state_chunks.getSize(); x++)
	for (y = 0; y < chunks.getSize(); y++) {
	  value = 1.0;
	  value2 = state_vector.getValue(x);
	  value *= value2;
	  value2 = chunks.getValue(y);
	  value *= value2;
	  success = success && state_chunks.setValue((x*chunks.getSize()) + y,
						     value);
	}
      
      // Fill conjunction part
      for (x = 0; x < state_chunks.getSize(); x++)
	setValue(x,state_chunks.getValue(x));
      
      // Concatenate OR vector
      for (y = 0; y < or_vector.getSize(); y++, x++)
	success = success && setValue(x,or_vector.getValue(y));
    }
    else {
      clearVector();
    }
  }
  
  if (afv_code == COMPLETE_CONJ) {
    if (number_of_chunks > 0) {
      for (x = 0; x < number_of_chunks; x++)
	c[x] = 0;
      
      int c_size = (int) pow((double) c_vector_size, number_of_chunks);
      FeatureVector chunks((int) pow((double) c_vector_size, number_of_chunks));

      if (number_of_chunks > 0) {
	double value;
	double value2;
	for (x = 0; x < c_size; x++) {
	  value = 1.0;
	  value2 = chunk_vectors[0]->getValue(c[0]);
	  value *= value2;
	  for (y = 1; y < number_of_chunks; y++) {
	    value2 = chunk_vectors[y]->getValue(c[y]);
	    value *= value2;
	  }
	  success = success && chunks.setValue(x, value);
	  
	  bool carry = true;
	  for (y = 1; y < number_of_chunks && carry; y++) {
	    c[y]++;
	    if (c[y] < c_vector_size)
	      carry = false;
	    else
	      c[y] = 0;
	  }
	  
	  if (carry)
	    c[0]++;
	}
      }
      
#ifdef WMTK_DEBUG
      cout << "Filling with: " << chunks << endl;
#endif

      z = 0;
      if (c_size == 0) {
	for (x = 0; x < state_vector.getSize(); x++) {
	  success = success && setValue(z++, state_vector.getValue(x));
	}
      }
      else {
	// Conjunct state vector
	for (x = 0; x < state_vector.getSize(); x++) {
	  for (y = 0; y < chunks.getSize(); y++)
	    success = success && setValue(z++, state_vector.getValue(x) * chunks.getValue(y));
	}
      }

      // Concatenate OR vector
      for (x = 0; x < or_vector.getSize(); x++)
	success = success && setValue(z++,or_vector.getValue(x));

    }
    else {
      clearVector();
      for (x = getSize() - or_vector.getSize(), y = 0; x < getSize(); x++, y++)
	success = success && setValue(x, or_vector.getValue(y));
    }
  }

  if (afv_code == WM_CONJ) {
    if (number_of_chunks > 0) {
      for (x = 0; x < number_of_chunks; x++)
	c[x] = 0;
      
      int c_size = (int) pow((double) c_vector_size, number_of_chunks);
      FeatureVector chunks((int) pow((double) c_vector_size, number_of_chunks));

      if (number_of_chunks > 0) {
	double value;
	double value2;
	for (x = 0; x < c_size; x++) {
	  value = 1.0;
	  value2 = chunk_vectors[0]->getValue(c[0]);
	  value *= value2;
	  for (y = 1; y < number_of_chunks; y++) {
	    value2 = chunk_vectors[y]->getValue(c[y]);
	    value *= value2;
	  }
	  success = success && chunks.setValue(x, value);
	  
	  bool carry = true;
	  for (y = 1; y < number_of_chunks && carry; y++) {
	    c[y]++;
	    if (c[y] < c_vector_size)
	      carry = false;
	    else
	      c[y] = 0;
	  }
	  
	  if (carry)
	    c[0]++;
	}
      }
      
#ifdef WMTK_DEBUG
      cout << "Filling with: " << chunks << endl;
#endif

      z = 0;
      // Fill state vector
      for (x = 0; x < state_vector.getSize(); x++) {
	success = success && setValue(z++, state_vector.getValue(x));
      }

      // Fill conjunction part
      for (x = 0; x < chunks.getSize(); x++)
	success = success && setValue(z++,chunks.getValue(x));

      // Concatenate OR vector
      for (x = 0; x < or_vector.getSize(); x++)
	success = success && setValue(z++,or_vector.getValue(x));

    }
    else {
      clearVector();
      for (x = getSize() - or_vector.getSize(), y = 0; x < getSize(); x++, y++)
	success = success && setValue(x, or_vector.getValue(y));
    }
  }

  if (afv_code == CONCAT) {

    // Fill in aggrgate vector with values from the state and chunk vectors
    z = 0; // Counter for AggregateVector position
    for (x = 0; x < state_vector.getSize(); x++)
      success = success && setValue(z++, state_vector.getValue(x));
    
    for (x = 0; x < number_of_chunks; x++)
      for (y = 0; y < chunk_vectors[x]->getSize(); y++)
	success = success && setValue(z++, chunk_vectors[x]->getValue(y));

    for (x = 0; x < or_vector.getSize(); x++)
      success = success && setValue(z++, or_vector.getValue(x));
  }

  // DEBUG
  /*
    cout << "Final Vector: ";
    for (x = 0; x < getSize(); x++)
    cout << getValue(x) << " ";
    cout << endl;
  */

  #ifdef WMTK_DEBUG
  cout << "Result: " << *(this) << endl;
  #endif

  // Return 'and' of all of the setValue calls. If one failed, we will know.
  delete [] c;
  return success;
}

int AggregateFeatureVector::determineNecessarySize(int state_size, int chunk_size, int num_chunks) {
  if (state_size < 0 || chunk_size < 0 || num_chunks < 0)
    return 0;

  // Since negatives have been eliminated, we can provide a decent size.
  // Notice that this allows for the state of chunk vectors to act
  // independently if needed. That is, we don't neccessary need a state
  // vector or list of chunk vectors, just one or the other or both. Thus,
  // if the updateFeatures function is called with a dummy StateFeatureVector
  // or a dummy list of ChunkFeatureVectors then this feature becomes
  // usable.

  // Concatenated coding
  if (afv_code == CONCAT)
    return (state_size + (num_chunks * chunk_size) + chunk_size);

  if (afv_code == WM_STATE_CONJ) {
    // This for WM/State conjunction
    if (state_size == 0 && (chunk_size == 0 || num_chunks == 0))
      return chunk_size;
    if (state_size == 0)
      return (chunk_size * num_chunks) + chunk_size;
    if (chunk_size == 0 || num_chunks == 0)
      return state_size;
    return (state_size * (chunk_size *  num_chunks)) + chunk_size;
  }

  if (afv_code == COMPLETE_CONJ) {
    // All of these are for complete conjunctive coding
    if (state_size == 0 && (chunk_size == 0 || num_chunks == 0))
      return chunk_size;
    if (state_size == 0)
      return ((int) pow((double) chunk_size, num_chunks)) + chunk_size;
    if (chunk_size == 0 || num_chunks == 0)
      return state_size + chunk_size;
    return (state_size * (int) pow((double) chunk_size, num_chunks)) +
      chunk_size;
  }

  if (afv_code == WM_CONJ) {
    // Conjunct just the WM contents
    if (state_size == 0 && (chunk_size == 0 || num_chunks == 0))
      return chunk_size;
    if (state_size == 0)
      return ((int) pow((double) chunk_size, num_chunks)) + chunk_size;
    if (chunk_size == 0 || num_chunks == 0)
      return state_size + chunk_size;
    return (state_size + (int) pow((double) chunk_size, num_chunks)) +
      chunk_size;
  }
  return 0;
}
