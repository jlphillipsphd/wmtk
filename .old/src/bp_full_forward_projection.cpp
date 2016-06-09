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
 * bp_full_forward_projection.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <bp_full_forward_projection.h>
#include <neural_unit.h>
#include <layer.h>
#include <nnet_math.h>
#include <cstddef>

BpFullForwardProjection::BpFullForwardProjection() : FullForwardProjection() {
  learning_rate = 0.3;
  momentum = 0.0;
  from_work_vec1 = NULL;
  from_work_vec2 = NULL;
  to_work_vec1 = NULL;
  last_updates = NULL;
}

void BpFullForwardProjection::init() {
  learning_rate = 0.3;
  momentum = 0.0;
  from_work_vec1 = NULL;
  from_work_vec2 = NULL;
  to_work_vec1 = NULL;
  last_updates = NULL;
}

void BpFullForwardProjection::dispose() {
  if (from_work_vec1 != NULL)
    delete [] from_work_vec1;

  if (from_work_vec2 != NULL)
    delete [] from_work_vec2;

  if (to_work_vec1 != NULL)
    delete [] to_work_vec1;

  if (last_updates != NULL) {
    for (int x = 0; x < getFromSize(); x++)
      delete [] last_updates[x];

    delete [] last_updates;
  }
}

BpFullForwardProjection::BpFullForwardProjection(const BpFullForwardProjection& L) : FullForwardProjection(L) {
  int x,y;
  init();
  learning_rate = L.learning_rate;
  momentum = L.momentum;
  if (L.from_work_vec1 != NULL) {
    from_work_vec1 = new double[getFromSize()];
    for (x = 0; x < getFromSize(); x++)
      from_work_vec1[x] = L.from_work_vec1[x];
  }
  if (L.from_work_vec2 != NULL) {
    from_work_vec2 = new double[getFromSize()];
    for (x = 0; x < getFromSize(); x++)
      from_work_vec2[x] = L.from_work_vec2[x];
  }
  if (L.to_work_vec1 != NULL) {
    to_work_vec1 = new double[getToSize()];
    for (x = 0; x < getToSize(); x++)
      to_work_vec1[x] = L.to_work_vec1[x];
  }
  if (L.last_updates != NULL) {
    last_updates = new double*[getFromSize()];
    for (x = 0; x < getFromSize(); x++) {
      last_updates[x] = new double[getToSize()];
      for (y = 0; y < getToSize(); y++)
	last_updates[x][y] = L.last_updates[x][y];
    }
  }
}

BpFullForwardProjection& BpFullForwardProjection::operator=(const BpFullForwardProjection& Rhs) {
  int x, y;

  if (this != &Rhs) {
    FullForwardProjection::operator=(Rhs);
    dispose();
    learning_rate = Rhs.learning_rate;
    momentum = Rhs.momentum;
    if (Rhs.from_work_vec1 != NULL) {
      from_work_vec1 = new double[getFromSize()];
      for (x = 0; x < getFromSize(); x++)
	from_work_vec1[x] = Rhs.from_work_vec1[x];
    }
    if (Rhs.from_work_vec2 != NULL) {
      from_work_vec2 = new double[getFromSize()];
      for (x = 0; x < getFromSize(); x++)
	from_work_vec2[x] = Rhs.from_work_vec2[x];
    }
    if (Rhs.to_work_vec1 != NULL) {
      to_work_vec1 = new double[getToSize()];
      for (x = 0; x < getToSize(); x++)
	to_work_vec1[x] = Rhs.to_work_vec1[x];
    }
    if (Rhs.last_updates != NULL) {
      last_updates = new double*[getFromSize()];
      for (x = 0; x < getFromSize(); x++) {
	last_updates[x] = new double[getToSize()];
	for (y = 0; y < getToSize(); y++)
	  last_updates[x][y] = Rhs.last_updates[x][y];
      }
    }
  }
  return *this;
}

BpFullForwardProjection::BpFullForwardProjection(Layer* from_layer, Layer* to_layer, int dimension) : FullForwardProjection(from_layer, to_layer, dimension) {
  learning_rate = 0.3;
  momentum = 0.0;

  if (from_layer->Size() > 0)
    from_work_vec1 = new double[from_layer->Size()];

  if (from_layer->Size() > 0)
    from_work_vec2 = new double[from_layer->Size()];

  if (to_layer->Size() > 0)
    to_work_vec1 = new double[to_layer->Size()];

  if (from_layer->Size() > 0 && to_layer->Size() > 0) {
    last_updates = new double*[from_layer->Size()];
    for (int x = 0; x < from_layer->Size(); x++) {
      last_updates[x] = new double[to_layer->Size()];
      for (int y = 0; y < to_layer->Size(); y++)
	last_updates[x][y] = 0.0;
    }
  }
}

BpFullForwardProjection::~BpFullForwardProjection() {
  dispose();
}

bool BpFullForwardProjection::setLearningRate(double val) {
  learning_rate = val;
  return true;
}

double BpFullForwardProjection::getLearningRate() {
  return learning_rate;
}

bool BpFullForwardProjection::setMomentum(double val) {
  momentum = val;
  return true;
}

double BpFullForwardProjection::getMomentum() {
  return momentum;
}

bool BpFullForwardProjection::computeNetDeltas() {

  if (weights == NULL || d_weights == NULL)
    return false;

  for (int x = 0; x < getToLayer()->Size(); x++) {
    getToLayer()->getUnit(x)->getDeltas(work_vec3);
    to_work_vec1[x] = work_vec3[getDimension()];
  }

  multiply(getToLayer()->Size(), to_work_vec1, getFromLayer()->Size(), w_transpose, from_work_vec1);

  getFromLayer()->setNetDeltas(from_work_vec1);

  return true;
}

bool BpFullForwardProjection::adjustNetDeltas() {

  if (weights == NULL || d_weights == NULL)
    return false;

  for (int x = 0; x < getToLayer()->Size(); x++) {
    getToLayer()->getUnit(x)->getDeltas(work_vec3);
    to_work_vec1[x] = work_vec3[getDimension()];
  }

  multiply(getToLayer()->Size(), to_work_vec1, getFromLayer()->Size(), w_transpose, from_work_vec1);

  getFromLayer()->getNetDeltas(from_work_vec2);

  for (int y = 0; y < getFromLayer()->Size(); y++)
    from_work_vec1[y] += from_work_vec2[y];

  getFromLayer()->setNetDeltas(from_work_vec1);

  return true;
}

bool BpFullForwardProjection::computeWeightChanges() {

  if (weights == NULL || d_weights == NULL)
    return false;

  int x,y;

  for (x = 0; x < getToLayer()->Size(); x++) {
    getToLayer()->getUnit(x)->getDeltas(work_vec3);
    to_work_vec1[x] = work_vec3[getDimension()];
  }
  getFromLayer()->getActs(from_work_vec1);

  for (x = 0; x < getFromLayer()->Size(); x++)
    for (y = 0; y < getToLayer()->Size(); y++) {
      d_weights[x][y] += (learning_rate * to_work_vec1[y] * from_work_vec1[x]) + (momentum * last_updates[x][y]);
      last_updates[x][y] = (learning_rate * to_work_vec1[y] * from_work_vec1[x]) + (momentum * last_updates[x][y]);
    }

  return true;

}

