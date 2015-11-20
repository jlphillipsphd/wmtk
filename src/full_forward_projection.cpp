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
 * full_forward_projection.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <full_forward_projection.h>
#include <neural_unit.h>
#include <simple_activation_function.h>
#include <random_number_generator.h>
#include <layer.h>
#include <nnet_math.h>
#include <cstddef>
#include <iostream>

FullForwardProjection::FullForwardProjection() : Projection() {
  init();
}

void FullForwardProjection::init() {
  min_w = -1.0;
  max_w = -1.0;
  w_bound = false;
  weights = NULL;
  d_weights = NULL;
  w_transpose = NULL;
  work_vec1 = NULL;
  work_vec2 = NULL;
  work_vec3 = NULL;
  work_vec4 = NULL;
  work_vec5 = NULL;
}

void FullForwardProjection::dispose() {
  int x;
  if (weights != NULL) {
    for (x = 0; x < getFromSize(); x++)
      delete [] weights[x];

    delete [] weights;
  }
  if (d_weights != NULL) {
    for (x = 0; x < getFromSize(); x++)
      delete [] d_weights[x];

    delete [] d_weights;
  }
  if (w_transpose != NULL) {
    for (x = 0; x < getToSize(); x++)
      delete [] w_transpose[x];

    delete [] w_transpose;
  }
  if (work_vec1 != NULL)
    delete [] work_vec1;
  if (work_vec2 != NULL)
    delete [] work_vec2;
  if (work_vec3 != NULL)
    delete [] work_vec3;
  if (work_vec4 != NULL)
    delete [] work_vec4;
  if (work_vec5 != NULL)
    delete [] work_vec5;
}

FullForwardProjection::~FullForwardProjection() {
  dispose();
}

FullForwardProjection::FullForwardProjection(const FullForwardProjection& L) : Projection(L) {
  int x,y;
  init();
  min_w = L.min_w;
  max_w = L.max_w;
  w_bound = L.w_bound;

  if (L.weights != NULL) {
    weights = new double*[getFromSize()];
    for (x = 0; x < getFromSize(); x++) {
      weights[x] = new double[getToSize()];
      for (y = 0; y < getToSize(); y++)
	weights[x][y] = L.weights[x][y];
    }
  }
  if (L.d_weights != NULL) {
    d_weights = new double*[getFromSize()];
    for (x = 0; x < getFromSize(); x++) {
      d_weights[x] = new double[getToSize()];
      for (y = 0; y < getToSize(); y++)
	d_weights[x][y] = L.d_weights[x][y];
    }
  }
  if (L.w_transpose != NULL) {
    w_transpose = new double*[getToSize()];
    for (x = 0; x < getToSize(); x++) {
      w_transpose[x] = new double[getFromSize()];
      for (y = 0; y < getFromSize(); y++)
	w_transpose[x][y] = L.w_transpose[x][y];
    }
  }
  if (L.work_vec1 != NULL) {
    work_vec1 = new double[getToSize()];
    for (x = 0; x < getToSize(); x++)
      work_vec1[x] = L.work_vec1[x];
  }
  if (L.work_vec2 != NULL) {
    work_vec2 = new double[getToSize()];
    for (x = 0; x < getToSize(); x++)
      work_vec2[x] = L.work_vec2[x];
  }
  if (L.work_vec3 != NULL) {
    work_vec3 = new double[getToLayer()->getNumberOfInputDimensions()];
    for (x = 0; x < getToLayer()->getNumberOfInputDimensions(); x++)
      work_vec3[x] = L.work_vec3[x];
  }
  if (L.work_vec4 != NULL) {
    work_vec4 = new double[getToLayer()->getNumberOfInputDimensions()];
    for (x = 0; x < getToLayer()->getNumberOfInputDimensions(); x++)
      work_vec4[x] = L.work_vec4[x];
  }
  if (L.work_vec5 != NULL) {
    work_vec5 = new double[getFromSize()];
    for (x = 0; x < getFromSize(); x++)
      work_vec5[x] = L.work_vec5[x];
  }
}



FullForwardProjection& FullForwardProjection::operator=(const FullForwardProjection& Rhs) {
  int x, y;
  if (this != &Rhs) {
    Projection::operator=(Rhs);
    dispose();
    min_w = Rhs.min_w;
    max_w = Rhs.max_w;
    w_bound = Rhs.w_bound;

    if (Rhs.weights != NULL) {
      weights = new double*[getFromSize()];
      for (x = 0; x < getFromSize(); x++) {
	weights[x] = new double[getToSize()];
	for (y = 0; y < getToSize(); y++)
	  weights[x][y] = Rhs.weights[x][y];
      }
    }
    if (Rhs.d_weights != NULL) {
      d_weights = new double*[getFromSize()];
      for (x = 0; x < getFromSize(); x++) {
	d_weights[x] = new double[getToSize()];
	for (y = 0; y < getToSize(); y++)
	  d_weights[x][y] = Rhs.d_weights[x][y];
      }
    }
    if (Rhs.w_transpose != NULL) {
      w_transpose = new double*[getToSize()];
      for (x = 0; x < getToSize(); x++) {
	w_transpose[x] = new double[getFromSize()];
	for (y = 0; y < getFromSize(); y++)
	  w_transpose[x][y] = Rhs.w_transpose[x][y];
      }
    }
    if (Rhs.work_vec1 != NULL) {
      work_vec1 = new double[getToSize()];
      for (x = 0; x < getToSize(); x++)
	work_vec1[x] = Rhs.work_vec1[x];
    }
    if (Rhs.work_vec2 != NULL) {
      work_vec2 = new double[getToSize()];
      for (x = 0; x < getToSize(); x++)
	work_vec2[x] = Rhs.work_vec2[x];
    }
    if (Rhs.work_vec3 != NULL) {
      work_vec3 = new double[getToLayer()->getNumberOfInputDimensions()];
      for (x = 0; x < getToLayer()->getNumberOfInputDimensions(); x++)
	work_vec3[x] = Rhs.work_vec3[x];
    }
    if (Rhs.work_vec4 != NULL) {
      work_vec4 = new double[getToLayer()->getNumberOfInputDimensions()];
      for (x = 0; x < getToLayer()->getNumberOfInputDimensions(); x++)
	work_vec4[x] = Rhs.work_vec4[x];
    }
    if (Rhs.work_vec5 != NULL) {
      work_vec5 = new double[getFromSize()];
      for (x = 0; x < getFromSize(); x++)
	work_vec5[x] = Rhs.work_vec5[x];
    }
  }
  return *this;
}

FullForwardProjection::FullForwardProjection(Layer* from_layer, Layer* to_layer, int dimension) : Projection(from_layer, to_layer, dimension) {

  if (from_layer == NULL || to_layer == NULL) {
    init();
    return;
  }

  min_w = -1.0;
  max_w = -1.0;
  w_bound = false;

  int from_size = from_layer->Size();
  int to_size = to_layer->Size();
  int x, y;

  weights = new double*[from_size];
  d_weights = new double*[from_size];
  w_transpose = new double*[to_size];
  work_vec1 = new double[to_size];
  work_vec2 = new double[to_size];
  work_vec3 = new double[to_layer->getNumberOfInputDimensions()];
  work_vec4 = new double[to_layer->getNumberOfInputDimensions()];
  work_vec5 = new double[from_size];

  for (y = 0; y < to_size; y++)
    w_transpose[y] = new double[from_size];

  for (x = 0; x < from_size; x++) {
    weights[x] = new double[to_size];
    d_weights[x] = new double[to_size];
    for (y = 0; y < to_size; y++) {
      weights[x][y] = 0.0;
      w_transpose[y][x] = 0.0;
      d_weights[x][y] = 0.0;
    }
  }

}

bool FullForwardProjection::initializeWeights( RandomNumberGenerator* rand ) {
  if (rand == NULL || weights == NULL || d_weights == NULL)
    return false;

  int x,y;

  for (x = 0; x < getFromLayer()->Size(); x++)
    for (y = 0; y < getToLayer()->Size(); y++) {
      weights[x][y] = rand->Number();
      weights[x][y] = bound(weights[x][y]);
      w_transpose[y][x] = weights[x][y];
    }

  return true;
}

bool FullForwardProjection::perturbWeights( RandomNumberGenerator* rand ) {
  if (rand == NULL || weights == NULL || d_weights == NULL)
    return false;

  int x,y;
  double val;

  for (x = 0; x < getFromLayer()->Size(); x++)
    for (y = 0; y < getToLayer()->Size(); y++) {
      val = rand->Number();
      weights[x][y] += val;
      weights[x][y] = bound(weights[x][y]);
      w_transpose[y][x] = weights[x][y];
    }

  return true;
}

bool FullForwardProjection::updateWeights() {

  if (d_weights == NULL || weights == NULL)
    return false;

  increment(getFromLayer()->Size(), getToLayer()->Size(), weights, d_weights);

  for (int x = 0; x < getFromLayer()->Size(); x++)
    for (int y = 0; y < getToLayer()->Size(); y++) {
      weights[x][y] = bound(weights[x][y]);
      w_transpose[y][x] = weights[x][y];
    }

  clearDeltaWeights();

  return true;
}

bool FullForwardProjection::clearDeltaWeights() {
  if (weights == NULL || d_weights == NULL)
    return false;

  for (int x = 0; x < getFromLayer()->Size(); x++)
    for (int y = 0; y < getToLayer()->Size(); y++)
      d_weights[x][y] = 0.0;

  return true;
}

bool FullForwardProjection::clearWeights() {
  if (weights == NULL || d_weights == NULL)
    return false;

  for (int x = 0; x < getFromLayer()->Size(); x++)
    for (int y = 0; y < getToLayer()->Size(); y++) {
      d_weights[x][y] = 0.0;
      weights[x][y] = 0.0;
      weights[x][y] = bound(weights[x][y]);
      w_transpose[y][x] = 0.0;
    }

  return true;
}

void FullForwardProjection::Print() {

  cout << "Projection information:" << endl;
  cout << "Weights:" << endl;
  print_matrix(getFromLayer()->Size(), getToLayer()->Size(), weights);
  cout << "Transpose Weights:" << endl;
  print_matrix(getToLayer()->Size(), getFromLayer()->Size(), w_transpose);
  cout << "Next Weight Adjustments:" << endl;
  print_matrix(getFromLayer()->Size(), getToLayer()->Size(), d_weights);
  cout << endl;

}

bool FullForwardProjection::writeWeights(ofstream& output_file) {

  if (weights == NULL)
    return false;

  for (int x = 0; x < getFromLayer()->Size(); x++) {
    for (int y =0; y < getToLayer()->Size(); y++)
      output_file << weights[x][y] << " ";

    output_file << endl;

  } // end for

  return true;
}

bool FullForwardProjection::readWeights(ifstream& input_file) {

  if (weights == NULL)
    return false;

  for (int x = 0; x < getFromLayer()->Size(); x++) {
    for (int y = 0; y < getToLayer()->Size(); y++) {
      input_file >> weights[x][y];
      weights[x][y] = bound(weights[x][y]);
      w_transpose[y][x] = weights[x][y];
    } // end for
  }

  return true;
}

bool FullForwardProjection::computeNets() {

  if (weights == NULL || d_weights == NULL)
    return false;

  getFromLayer()->getActs(work_vec5);

  multiply(getFromLayer()->Size(), work_vec5, getToLayer()->Size(), weights, work_vec1);

  for (int y = 0; y < getToLayer()->Size(); y++) {
    getToLayer()->getUnit(y)->getNets(work_vec3);
    work_vec3[getDimension()] = work_vec1[y];
    getToLayer()->getUnit(y)->setNets(work_vec3);
  }

  return true;

}

bool FullForwardProjection::adjustNets() {

  if (weights == NULL || d_weights == NULL)
    return false;

  getFromLayer()->getActs(work_vec5);

  multiply(getFromLayer()->Size(), work_vec5, getToLayer()->Size(), weights, work_vec1);

  for (int y = 0; y < getToLayer()->Size(); y++) {
    getToLayer()->getUnit(y)->getNets(work_vec3);
    work_vec3[getDimension()] += work_vec1[y];
    getToLayer()->getUnit(y)->setNets(work_vec3);
  }

  return true;

}

double FullForwardProjection::getWeight(int from_layer_pos, int to_layer_pos) {
  if (weights == NULL)
    return false;

  if (from_layer_pos < 0 || from_layer_pos >= getFromLayer()->Size())
    return false;

  if (to_layer_pos < 0 || to_layer_pos >= getToLayer()->Size())
    return false;

  return weights[from_layer_pos][to_layer_pos];
}

bool FullForwardProjection::setWeight(int from_layer_pos, int to_layer_pos, double val) {
  if (weights == NULL)
    return false;

  if (from_layer_pos < 0 || from_layer_pos >= getFromLayer()->Size())
    return false;

  if (to_layer_pos < 0 || to_layer_pos >= getToLayer()->Size())
    return false;

  val = bound(val);
  weights[from_layer_pos][to_layer_pos] = val;
  w_transpose[to_layer_pos][from_layer_pos] = val;

  return true;
}

double FullForwardProjection::bound(double val) {
  if (w_bound) {
    if (val < min_w)
      return min_w;
    if (val > max_w)
      return max_w;
  }
  else
    return val;
}

void FullForwardProjection::useWeightBounding(bool val) {
  if (val && w_bound)
    return;

  if (!val && !w_bound)
    return;

  w_bound = val;

  // Apply
  if (w_bound) {
    for (int x = 0; x < getFromLayer()->Size(); x++)
      for (int y = 0; y < getToLayer()->Size(); y++) {
	weights[x][y] = bound(weights[x][y]);
	w_transpose[y][x] = weights[x][y];
      }
  }

}

double FullForwardProjection::getWeightBoundMinimum() {
  return min_w;
}

double FullForwardProjection::getWeightBoundMaximum() {
  return max_w;
}

bool FullForwardProjection::setWeightBoundMinimum(double val) {
  if (val >= max_w)
    return false;

  min_w = val;

  if (w_bound) {
    for (int x = 0; x < getFromLayer()->Size(); x++)
      for (int y = 0; y < getToLayer()->Size(); y++) {
	weights[x][y] = bound(weights[x][y]);
	w_transpose[y][x] = weights[x][y];
      }
  }

  return true;
}

bool FullForwardProjection::setWeightBoundMaximum(double val) {
  if (val <= min_w)
    return false;

  max_w = val;

  if (w_bound) {
    for (int x = 0; x < getFromLayer()->Size(); x++)
      for (int y = 0; y < getToLayer()->Size(); y++) {
	weights[x][y] = bound(weights[x][y]);
	w_transpose[y][x] = weights[x][y];
      }
  }

  return true;
}

