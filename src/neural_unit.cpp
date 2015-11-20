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
 * neural_unit.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <neural_unit.h>
#include <cstddef>
#include <iostream>

NeuralUnit::NeuralUnit() : NNetObject() {
  init();
}

NeuralUnit::~NeuralUnit() {
  dispose();
} // destructor

void NeuralUnit::dispose() {
  delete [] nets;
  delete [] deltas;
  delete [] derivatives;
  if (position != NULL)
    delete [] position;
}

void NeuralUnit::init() {
  act = 0.0;
  target = 0.0;
  net_delta = 0.0;
  nets = new double[1];
  nets[0] = 0.0;
  deltas = new double[1];
  deltas[0] = 0.0;
  derivatives = new double[1];
  derivatives[0] = 0.0;
  position = NULL;
  p_dims = 0;
  n_dims = 1;
}

NeuralUnit::NeuralUnit(const NeuralUnit& L) {
  int x;
  act = L.act;
  target = L.target;
  net_delta = L.net_delta;
  nets = new double[L.n_dims];
  for (x = 0; x < L.n_dims; x++)
    nets[x] = L.nets[x];
  deltas = new double[L.n_dims];
  for (x = 0; x < L.n_dims; x++)
    deltas[x] = L.deltas[x];
  derivatives = new double[L.n_dims];
  for (x = 0; x < L.n_dims; x++)
    derivatives[x] = L.derivatives[x];
  if (L.p_dims == 0) {
    position = NULL;
    p_dims = 0;
  }
  else {
    position = new double[L.p_dims];
    for (x = 0; x < L.p_dims; x++)
      position[x] = L.position[x];
    p_dims = L.p_dims;
  }

  n_dims = L.n_dims;
}

NeuralUnit::NeuralUnit(int number_of_input_dimensions) : NNetObject() {

  int x = number_of_input_dimensions;
  int i;

  if (x < 1) {
    init();
    return;
  }

  act = 0.0;
  target = 0.0;
  net_delta = 0.0;

  nets = new double[x];
  for (i = 0; i < x; i++)
    nets[i] = 0.0;

  deltas = new double[x];
  for (i = 0; i < x; i++)
    deltas[i] = 0.0;

  derivatives = new double[x];
  for (i = 0; i < x; i++)
    derivatives[i] = 0.0;

  position = NULL;
  p_dims = 0;
  n_dims = x;

}

NeuralUnit& NeuralUnit::operator=(const NeuralUnit& Rhs) {
  int x;

  if (this != &Rhs) {
    NNetObject::operator=(Rhs);
    dispose();
    act = Rhs.act;
    target = Rhs.target;
    net_delta = Rhs.net_delta;
    nets = new double[Rhs.n_dims];
    for (x = 0; x < Rhs.n_dims; x++)
      nets[x] = Rhs.nets[x];
    deltas = new double[Rhs.n_dims];
    for (x = 0; x < Rhs.n_dims; x++)
      deltas[x] = Rhs.deltas[x];
    derivatives = new double[Rhs.n_dims];
    for (x = 0; x < Rhs.n_dims; x++)
      derivatives[x] = Rhs.derivatives[x];
    if (Rhs.p_dims == 0) {
      position = NULL;
      p_dims = 0;
    }
    else {
      position = new double[Rhs.p_dims];
      for (x = 0; x < Rhs.p_dims; x++)
	position[x] = Rhs.position[x];
      p_dims = Rhs.p_dims;
    }

    n_dims = Rhs.n_dims;
  }
  return *this;
}

bool NeuralUnit::setAct(double val) {
  act = val;
  return true;
}

double NeuralUnit::getAct() {
  double val;
  val = act;
  return val;
}

bool NeuralUnit::setTarget(double val) {
  target = val;
  return true;
}

double NeuralUnit::getTarget() {
  double val;
  val = target;
  return val;
}

bool NeuralUnit::setNets(double vals[]) {

  if (nets == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    nets[x] = vals[x];

  return true;
}

bool NeuralUnit::getNets(double vals[]) {

  if (nets == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    vals[x] = nets[x];

  return true;
}

bool NeuralUnit::setDerivatives(double vals[]) {

  if (derivatives == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    derivatives[x] = vals[x];

  return true;
}

bool NeuralUnit::getDerivatives(double vals[]) {

  if (derivatives == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    vals[x] = derivatives[x];

  return true;
}

bool NeuralUnit::setNetDelta(double val) {
  net_delta = val;
  return true;
}

double NeuralUnit::getNetDelta() {
  double val;
  val = net_delta;
  return val;
}

bool NeuralUnit::setDeltas(double vals[]) {

  if (deltas == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    deltas[x] = vals[x];

  return true;
}

bool NeuralUnit::getDeltas(double vals[]) {

  if (deltas == NULL || vals == NULL) {
    return false;
  }

  for (int x = 0; x < n_dims; x++)
    vals[x] = deltas[x];

  return true;
}

bool NeuralUnit::setPositionDimensions(int val) {

  if (val <= 0)
    return false;


  if (position != NULL)
    delete [] position;

  position = new double[val];
  p_dims = val;

  return true;
}

int NeuralUnit::getPositionDimensions() {
  int val;
  val = p_dims;
  return val;
}

double NeuralUnit::getPosition(int dim) {
  double val;
  if (position == NULL || dim > p_dims || dim < 0) {
    return 0.0;
  }

  val = position[dim];
  return val;
}

bool NeuralUnit::setPosition(int dim, double val) {

  if (position == NULL || dim > p_dims || dim < 0) {
    return false;
  }

  position[dim] = val;
  return true;
}

bool NeuralUnit::getPosition(double vals[]) {

  if (position == NULL) {
    return false;
  }

  for (int x = 0; x < p_dims; x++)
    vals[x] = position[x];

  return true;
}

bool NeuralUnit::setPosition(double vals[]) {

  if (position == NULL) {
    return false;
  }

  for (int x = 0; x < p_dims; x++)
    position[x] = vals[x];

  return true;
}

int NeuralUnit::getInputDimensions() {
  int val;
  val = n_dims;
  return val;
}
