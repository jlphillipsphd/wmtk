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
 * projection.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <projection.h>
#include <layer.h>
#include <cstddef>
#include <iostream>

Projection::Projection() : NNetObject() {
  init();
}

Projection::Projection(const Projection& L) : NNetObject(L) {
  from = L.from;
  to = L.to;
  dim = L.dim;
  fl_size = L.fl_size;
  tl_size = L.tl_size;
}

Projection::Projection(Layer* from_layer, Layer* to_layer, int dimension) : NNetObject() {
  if (from_layer == NULL || to_layer == NULL) {
    init();
    return;
  }

  if (dimension < 0 || dimension >= to_layer->getNumberOfInputDimensions()) {
    init();
    return;
  }

  dim = dimension;
  from = from_layer;
  to = to_layer;
  tl_size = to_layer->Size();
  fl_size = from_layer->Size();
}

void Projection::init() {
  from = NULL;
  to = NULL;
  dim = -1;
}

Projection& Projection::operator=(const Projection& Rhs) {
  if (this != &Rhs) {
    NNetObject::operator=(Rhs);
    from = Rhs.from;
    to = Rhs.to;
    dim = Rhs.dim;
    fl_size = Rhs.fl_size;
    tl_size = Rhs.tl_size;
  }
  return *this;
}

Layer* Projection::getFromLayer() {
  return from;
}

Layer* Projection::getToLayer() {
  return to;
}

int Projection::getToSize() {
  return tl_size;
}

int Projection::getFromSize() {
  return fl_size;
}

int Projection::getDimension() {
  return dim;
}
