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
 * td_layer.cpp
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#include <td_layer.h>
#include <activation_function.h>
#include <cstddef>
#include <iostream>
#include <iomanip>

TDLayer::TDLayer() : Layer() {
  init();
}

TDLayer::~TDLayer() {
  dispose();
} // destructor

void TDLayer::init() {
  gamma = 1.0;
  last_acts = NULL;
  td_error = NULL;
  some_vec = NULL;
}

void TDLayer::dispose() {
  if (some_vec != NULL)
    delete [] some_vec;

  if (td_error != NULL)
    delete [] td_error;

  if (last_acts != NULL)
    delete [] last_acts;
}

TDLayer::TDLayer(const TDLayer& L) : Layer(L) {
  int x;
  init();
  if (Size() > 0) {
    some_vec = new double[Size()];
    for (x = 0; x < Size(); x++)
      some_vec[x] = L.some_vec[x];
    td_error = new double[Size()];
    for (x = 0; x < Size(); x++)
      td_error[x] = L.some_vec[x];
    last_acts = new double[Size()];
    for (x = 0; x < Size(); x++)
      last_acts[x] = L.last_acts[x];
  }
}

TDLayer& TDLayer::operator=(const TDLayer& Rhs) {
  int x;
  if (this != &Rhs) {
    Layer::operator=(Rhs);
    dispose();
    init();
    if (Size() > 0) {
      some_vec = new double[Size()];
      for (x = 0; x < Size(); x++)
	some_vec[x] = Rhs.some_vec[x];
      td_error = new double[Size()];
      for (x = 0; x < Size(); x++)
	td_error[x] = Rhs.some_vec[x];
      last_acts = new double[Size()];
      for (x = 0; x < Size(); x++)
	last_acts[x] = Rhs.last_acts[x];
    }
		
  }
  return *this;
}
TDLayer::TDLayer(int number_of_units, ActivationFunction*
		 saf, double discount_rate) : Layer(number_of_units, saf) {

  if (number_of_units <= 0) {
    init();
    return;
  }

  gamma = discount_rate;
  some_vec = new double[number_of_units];
  td_error = new double[number_of_units];
  last_acts = new double[number_of_units];

  for (int x = 0; x < Size(); x++) {
    td_error[x] = 0.0;
    last_acts[x] = 0.0;
  }
}

bool TDLayer::computeTDError(double* reward) {
  return computeTDError(reward, false);
}

bool TDLayer::computeTDError(double* reward, bool absorb_reward) {
  if (last_acts == NULL)
    return false;

  int x;

  getActs(some_vec);
  for (x = 0; x < Size(); x++) {
    if (!absorb_reward) {
      td_error[x] = reward[x] + (gamma * some_vec[x]) - last_acts[x];
    }
    else {
      td_error[x] = reward[x] - last_acts[x];
    }
  }

  return true;
}

bool TDLayer::setDiscountRate(double g) {
  gamma = g;
  return true;
}

double TDLayer::getDiscountRate() {
  return gamma;
}

bool TDLayer::shiftActivationState() {
  if (td_error == NULL)
    return false;

  getActs(some_vec);
  for (int x = 0; x < Size(); x++) {
    last_acts[x] = some_vec[x];
  }

  return true;
}

bool TDLayer::shiftActivationState(int index) {
  if (td_error == NULL || index < 0 || index >= Size())
    return false;

  getActs(some_vec);
  last_acts[index] = some_vec[index];

  return true;
}
bool TDLayer::getTDError(double* vals) {
  if (td_error == NULL)
    return false;

  for (int x = 0; x < Size(); x++)
    vals[x] = td_error[x];

  return true;;
}

bool TDLayer::setTDError(int index, double val) {
  if (td_error == NULL || index < 0 || index >= Size())
    return false;

  td_error[index] = val;
  return true;
}

double TDLayer::getTDError(int index) {
  if (td_error == NULL || index < 0 || index >= Size())
    return false;

  return td_error[index];
}

