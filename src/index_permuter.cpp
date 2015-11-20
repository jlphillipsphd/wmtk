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
 * index_permuter.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <index_permuter.h>
#include <cstdlib>
//#include <iostream>

IndexPermuter::IndexPermuter() : NNetObject() {
  min = 0;
  max = 1;
}

IndexPermuter::IndexPermuter(int minimum_index, int maximum_index) : NNetObject() {

  min = 0;
  max = 1;

  if (minimum_index < maximum_index) {
    int_interval i;
    i.min = minimum_index;
    i.max = maximum_index;
    intervals.push_back(i);
	
    min = minimum_index;
    max = maximum_index;
  }
}

bool IndexPermuter::reset() {

  int size = intervals.size();
  for (int x = 0; x < size; x++)
    intervals.pop_front();

  int_interval i;
  i.min = min;
  i.max = max;
  intervals.push_back(i);

  return true;
}

int IndexPermuter::getMinimumIndex() {
  return min;
}

int IndexPermuter::getMaximumIndex() {
  return max;
}

bool IndexPermuter::setMinimumIndex(int index) {
  if (max > index) {
    min = index;
    return true;
  }

  return false;
}

bool IndexPermuter::setMaximumIndex(int index) {
  if (min < index) {
    max = index;
    return true;
  }

  return false;
}

int IndexPermuter::getNextIndex() {

  if (intervals.size() == 0)
    return (min - 1);

  int pieces = intervals.size();
  int* index = new int[pieces];
  int chosen_piece;
  int return_val;
  list<int_interval>::iterator itr;
  int_interval i;
  int_interval top;
  int_interval bottom;
  int x;

  //cout << "Divisions: " << pieces << endl;
  //cout << "Possible values: " << endl;
  for (x = 0, itr = intervals.begin(); x < pieces; x++, itr++) {

    // Find span
    chosen_piece = (*itr).max - (*itr).min;

    if (chosen_piece == 0) {
      index[x] = (*itr).min;
    }
    else {
      // Choose a value from the span
      return_val = (rand() % (chosen_piece + 1));
      index[x] = (*itr).min + return_val;
    }

    //cout << x << " :  " << index[x] << endl;
  }

  chosen_piece = (rand() % pieces);
  return_val = index[chosen_piece];

  //cout << "Chosen: " << chosen_piece << " Value: " << return_val << endl;
  for (x = 0, itr = intervals.begin(); x < chosen_piece; x++, itr++);
  i = (*itr);
  intervals.erase(itr);

  if (i.max != i.min) {
    top.max = i.max;
    top.min = return_val + 1;
    bottom.min = i.min;
    bottom.max = return_val - 1;

    if (top.max >= top.min)
      intervals.push_back(top);
    if (bottom.max >= bottom.min)
      intervals.push_back(bottom);
  }

  delete [] index;
  return return_val;
}
