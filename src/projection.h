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
 * projection.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef PROJECTION_H
#define PROJECTION_H

#include <nnet_object.h>

class Layer;

#ifndef WIN32
using namespace std;
#endif

class Projection : public NNetObject {

public:
  Projection();
  Projection(const Projection&);
  Projection(Layer* from_layer, Layer* to_layer, int dimension);
  Projection& operator=(const Projection&);

  Layer* getFromLayer();
  Layer* getToLayer();

  int getFromSize();
  int getToSize();
  int getDimension();

  virtual bool computeNets() = 0;
  virtual bool adjustNets() = 0;
  virtual void Print() = 0;

private:
  Layer* from;
  Layer* to;
  int dim;
  int fl_size;
  int tl_size;

  void init();

};

#endif
