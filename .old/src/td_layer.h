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
 * td_layer.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef TD_LAYER_H
#define TD_LAYER_H

#include <layer.h>

class ActivationFunction;

#ifndef WIN32
using namespace std;
#endif

class TDLayer : public Layer {

private:
  double gamma;
  double* last_acts;
  double* td_error;
  double* some_vec;

  void init();
  void dispose();

public:
  TDLayer();
  TDLayer(const TDLayer&);
  ~TDLayer();
  TDLayer(int number_of_units, ActivationFunction* saf, double discount_rate);
  TDLayer& operator=(const TDLayer&);

  // NOTE: TD error is a GLOBAL error measure.
  bool computeTDError(double* reward);
  bool computeTDError(double* reward, bool absorb_reward);

  bool shiftActivationState();
  // Just shifts a single unit's activation only
  bool shiftActivationState(int index);

  bool setDiscountRate(double g);
  double getDiscountRate();

  bool getTDError(double* vals);
  bool setTDError(int index, double val);
  double getTDError(int index);

};

#endif
