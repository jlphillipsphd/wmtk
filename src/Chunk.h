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
 * Chunk.h
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class holds onto a particular memory trace or chunk so that it can be
 * transferred in and out of the WorkingMemory class's store. An informal
 * amount of type information can be associated with the chunk in order to
 * later identify the type of data it represents. This information is stored
 * in a simple string format to be used in any way the user desires. A list of
 * Chunk class objects will need to be made by the user and sent to the
 * WorkingMemory class when calling tickEpisodeClock.
 *
 * JLP - 5/5/04 6:55PM
 * 		Created stub class for developing FeatureVector class
 *              and subclasses.
 *
 * JLP - 5/25/04 1:32PM
 * 		Finished class prototype.
 *
 * JLP - 5/25/04 2:29PM
 * 		Added destructor.
 *
 *****************************************************************************/

#ifndef WMTK_CHUNK_H
#define WMTK_CHUNK_H

#include <string>

using namespace std;

class Chunk {

 public:

  // Constructor
  Chunk();

  // Constructor
  // Creates a chunk class with the specified data and type information.
  Chunk(void* data, string type);

  // Destructor
  ~Chunk();

  // Copy-Constructor
  Chunk(const Chunk&);

  // Assignment Operator
  Chunk& operator=(const Chunk&);

  // Returns a pointer to the chunk data.
  void* getData() const;

  // Set the current data pointer for this chunk class.
  void setData(void* data);

  // Returns the type string provided to the chunk class.
  string getType() const;

  // Sets the type string for the chunk.
  void setType(string type);

 private:
  // Pointer to chunk data.
  void*	chunk_data;

  // String tag containing some kind of user-specified type information.
  string chunk_info;
};

#endif
