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
 * full_forward_projection.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef FULL_FORWARD_PROJECTION_H
#define FULL_FORWARD_PROJECTION_H

#include <projection.h>
#include <fstream>

class Layer;
class RandomNumberGenerator;

using namespace std;

class FullForwardProjection : public Projection {

 private:
  double bound(double val);

  void init();
  void dispose();

 protected:
  double** weights;
  double** d_weights;
  double** w_transpose;
  double* work_vec1;
  double* work_vec2;
  double* work_vec3;
  double* work_vec4;
  double* work_vec5;
  double min_w;
  double max_w;
  bool w_bound;
	
 public:
  FullForwardProjection();
  FullForwardProjection(const FullForwardProjection&);
  ~FullForwardProjection();
  FullForwardProjection(Layer* from_layer, Layer* to_layer, int dimension);
  FullForwardProjection& operator=(const FullForwardProjection&);

  bool initializeWeights(RandomNumberGenerator* rand);
  bool perturbWeights(RandomNumberGenerator* rand);
  bool updateWeights();
  bool clearDeltaWeights();
  bool clearWeights();

  bool computeNets();

  bool adjustNets();

  bool setWeight(int from_layer_pos, int to_layer_pos, double val);
  double getWeight(int from_layer_pos, int to_layer_pos);

  void useWeightBounding(bool val);

  double getWeightBoundMinimum();
  bool setWeightBoundMinimum(double val);
  double getWeightBoundMaximum();
  bool setWeightBoundMaximum(double val);

  bool writeWeights(ofstream& output_file);
  bool readWeights(ifstream& input_file);
  void Print();

};

#endif
