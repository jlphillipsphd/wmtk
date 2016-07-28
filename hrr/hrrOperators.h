#ifndef HRR_OPERATORS
#define HRR_OPERATORS

//  Holographic Reduced Representation Engine (HRRE)
//  Copyright (C) Joshua L. Phillips & Grayson Dubois
//  Department of Computer Science
//  Middle Tennessee State University; Murfreesboro, Tennessee, USA.

//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTIBILITY or FITNESS FOR A PARTICULAT PURPOSE. See the
//  GNU General Public License for more details.

/******************************************************************************
 * hrrOperators.cpp
 *
 * This software is part of the Holographic Reduced Representation Engine.
 *
 * This library provides overloaded operators for HRRs to make HRR manipulation
 * easier. Implements the following operators: +, -
 *
 * Author:    Grayson M. Dubois
 * Mentor:    Joshua L. Phillips
 ******************************************************************************/

#include <vector>
#include <iostream>

typedef std::vector<double> HRR;

// Overload the '+' operator for HRRs to be the linear addition of two vectors
HRR operator+(const HRR& hrr1, const HRR& hrr2);

// Overload the '-' operator for HRRs to be the linear subtraction of two vectors
HRR operator-(const HRR& hrr1, const HRR& hrr2);

// Overload the '*' operator for HRRs to be circular convolution of two vectors
HRR operator*(const HRR& hrr1, const HRR& hrr2);

// Overload the '*' operator for an HRR and a scalar
HRR operator*(const HRR& hrr, const double c);

#endif  /*  HRR_OPERATORS  */
