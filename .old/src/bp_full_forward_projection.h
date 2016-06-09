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
 * bp_full_forward_projection.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef BP_FULL_FORWARD_PROJECTION_H
#define BP_FULL_FORWARD_PROJECTION_H

#include <full_forward_projection.h>

class Layer;

class BpFullForwardProjection : public FullForwardProjection {

 private:
  void init();
  void dispose();

 protected:
  double learning_rate;
  double momentum;
  double* from_work_vec1;
  double* from_work_vec2;
  double* to_work_vec1;
  double** last_updates;

 public:
  BpFullForwardProjection();
  BpFullForwardProjection(const BpFullForwardProjection&);
  ~BpFullForwardProjection();
  BpFullForwardProjection(Layer* from_layer, Layer* to_layer, int dimension);
  BpFullForwardProjection& operator=(const BpFullForwardProjection&);

  bool setLearningRate(double val);
  double getLearningRate();

  bool setMomentum(double val);
  double getMomentum();

  bool computeNetDeltas();
  bool adjustNetDeltas();

  bool computeWeightChanges();
};

#endif
