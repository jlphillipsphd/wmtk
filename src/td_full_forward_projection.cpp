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
 * td_full_forward_projection.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <td_full_forward_projection.h>
#include <neural_unit.h>
#include <nnet_math.h>
#include <td_layer.h>
#include <layer.h>
#include <cstddef>
#include <iostream>

TDFullForwardProjection::TDFullForwardProjection() : BpFullForwardProjection() {
  init();
}

TDFullForwardProjection::~TDFullForwardProjection() {
  dispose();
}

void TDFullForwardProjection::init() {
  e_traces = NULL;
  lambda = 0.0;
  critic_layer = NULL;
}

void TDFullForwardProjection::dispose() {
  if (e_traces != NULL) {
    for (int x = 0; x < getFromSize(); x++) {
      for (int y = 0; y < getToSize(); y++)
	delete [] e_traces[x][y];
      delete [] e_traces[x];
    }
    delete [] e_traces;
  }
}

TDFullForwardProjection::TDFullForwardProjection(const TDFullForwardProjection& L) : BpFullForwardProjection(L) {
  int x,y,z;
  init();
  lambda = L.lambda;
  critic_layer = L.critic_layer;
  if (L.e_traces != NULL) {
    e_traces = new double**[getFromSize()];
    for (x = 0; x < getFromSize(); x++) {
      e_traces[x] = new double*[getToSize()];
      for (y = 0; y < getToSize(); y++) {
	e_traces[x][y] = new double[critic_layer->Size()];
	for (z = 0; z < critic_layer->Size(); z++)
	  e_traces[x][y][z] = L.e_traces[x][y][z];
      }
    }
  }
}

TDFullForwardProjection& TDFullForwardProjection::operator=(const TDFullForwardProjection& Rhs) {
  int x,y,z;
  if (this != &Rhs) {
    BpFullForwardProjection::operator=(Rhs);
    dispose();
    lambda = Rhs.lambda;
    critic_layer = Rhs.critic_layer;
    if (Rhs.e_traces != NULL) {
      e_traces = new double**[getFromSize()];
      for (x = 0; x < getFromSize(); x++) {
	e_traces[x] = new double*[getToSize()];
	for (y = 0; y < getToSize(); y++) {
	  e_traces[x][y] = new double[critic_layer->Size()];
	  for (z = 0; z < critic_layer->Size(); z++)
	    e_traces[x][y][z] = Rhs.e_traces[x][y][z];
	}
      }
    }
  }
  return *this;
}

TDFullForwardProjection::TDFullForwardProjection(Layer* from_layer, Layer*
						 to_layer, int dimension, double lambda_value, TDLayer* critic_output_layer) : BpFullForwardProjection(from_layer, to_layer, dimension) {

  int x,y,z;

  if (from_layer == NULL || to_layer == NULL || critic_output_layer == NULL) {
    init();
    return;
  }

  critic_layer = critic_output_layer;
  lambda = lambda_value;

  if (lambda_value < 0.0 || lambda_value > 1.0)
    lambda = 0.0;

  e_traces = new double**[from_layer->Size()];
  for (x = 0; x < from_layer->Size(); x++) {
    e_traces[x] = new double*[to_layer->Size()];
    for (y = 0; y < to_layer->Size(); y++) {
      e_traces[x][y] = new double[critic_layer->Size()];
      for (z = 0; z < critic_layer->Size(); z++)
	e_traces[x][y][z] = 0.0;
    }
  }

}

bool TDFullForwardProjection::updateEligibilityTraces(int critic_unit) {
  if (weights == NULL || e_traces == NULL)
    return false;

  if (critic_unit < 0 || critic_unit >= critic_layer->Size())
    return false;
	
  int x,y;

  getFromLayer()->getActs(from_work_vec1);

  // Update e-traces
  for (y = 0; y < getToLayer()->Size(); y++) {
    getToLayer()->getUnit(y)->getDeltas(work_vec3);
    for (x = 0; x < getFromLayer()->Size(); x++) {
      e_traces[x][y][critic_unit] = (critic_layer->getDiscountRate() * lambda * e_traces[x][y][critic_unit]) + (work_vec3[getDimension()] * from_work_vec1[x]);
    }
  }

  return true;
}

bool TDFullForwardProjection::setLambda(double val) {
  if (val < 0.0 || val > 1.0)
    return false;

  lambda = val;
  return true;
}

double TDFullForwardProjection::getLambda() {
  return lambda;
}

bool TDFullForwardProjection::computeWeightChanges() {
  if (critic_layer == NULL)
    return false;

  for (int x = 0; x < critic_layer->Size(); x++) {
    if (!computeWeightChanges(x))
      return false;
  }

  return true;
	
}
bool TDFullForwardProjection::computeWeightChanges(int critic_unit) {

  if (weights == NULL || d_weights == NULL || e_traces == NULL )
    return false;

  if (critic_unit < 0 || critic_unit >= critic_layer->Size())
    return false;

  int x, y;
  double error = critic_layer->getTDError(critic_unit);

  // Compute weight updates
  for (x = 0; x < getFromLayer()->Size(); x++)
    for (y = 0; y < getToLayer()->Size(); y++)
      d_weights[x][y] += getLearningRate() * error * e_traces[x][y][critic_unit];

  return true;

}

bool TDFullForwardProjection::clearEligibilityTraces() {
  if (e_traces == NULL)
    return false;

  for (int x = 0; x < getFromLayer()->Size(); x++)
    for (int y = 0; y < getToLayer()->Size(); y++)
      for (int z = 0; z < critic_layer->Size(); z++) {
	e_traces[x][y][z] = 0.0;
      }

  return true;
}

void TDFullForwardProjection::printEligibilityTraces() {
  if (e_traces == NULL)
    return;

  double** buffer;
  int x,y;

  buffer = new double*[getFromLayer()->Size()];
  for (x = 0; x < getFromLayer()->Size(); x++)
    buffer[x] = new double[getToLayer()->Size()];

  for (int z = 0; z < critic_layer->Size(); z++) {
    for (x = 0; x < getFromLayer()->Size(); x++) {
      for (y = 0; y < getToLayer()->Size(); y++) {
	buffer[x][y] = e_traces[x][y][z];
      }
    }

    cout << "Eligibility traces for critic unit #" << z << endl;
    cout << "E-traces:" << endl;
    print_matrix(getFromLayer()->Size(), getToLayer()->Size(), buffer);
    cout << endl;

  }

  for (x = 0; x < getFromLayer()->Size(); x++)
    delete [] buffer[x];
  delete [] buffer;

  return;
}
