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
 * Chunk.cpp
 *
 * This software is part of the NSF ITR Robot-PFC Working Memory Toolkit.
 *
 * This class holds onto a particular memory trace or chunk so that it can be
 * transferred in and out of the WorkingMemory class's store. An informal
 * amount of type information can be associated with the chunk in order to
 * later identify the type of data it represents. This information is stored
 * in a simple string format to be used in any way the user desires. A list of
 * Chunk class objects will need to be made by the user and sent to the
 * WorkingMemory class when calling tickEpisodeClock (as shown above).
 *
 * JLP - 5/25/04 1:27PM
 * 		Source file created.
 *
 * JLP - 5/25/04 1:42 PM
 * 		Completed coding Chunk class.
 *
 * JLP - 5/25/04 2:29PM
 * 		Added Destructor.
 *
 *****************************************************************************/

#include <Chunk.h>
#include <cstddef>

Chunk::Chunk() {
  chunk_data = NULL;
  chunk_info = "NULL";
}

Chunk::~Chunk() {
  return;
}

Chunk::Chunk(void* data, string type) {
  chunk_data = data;
  chunk_info = type;
}

Chunk::Chunk(const Chunk& L) {
  chunk_data = L.chunk_data;
  chunk_info = L.chunk_info;
}

Chunk& Chunk::operator=(const Chunk& Rhs) {
  if (this != &Rhs) {
    chunk_data = Rhs.chunk_data;
    chunk_info = Rhs.chunk_info;
  }
  return *this;
}

void* Chunk::getData() const {
  return chunk_data;
}

void Chunk::setData(void* data) {
  chunk_data = data;
}

string Chunk::getType() const {
  return chunk_info;
}

void Chunk::setType(string type) {
  chunk_info = type;
}

