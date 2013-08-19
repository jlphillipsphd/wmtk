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
 * nnet_math.h
 *
 * This software was part of the LIBNNET project,
 * written by Joshua L. Phillips.
 *
 * This version has been ported for use in the Working Memory
 * Toolkit (WMtk). It was supplied AS-IS and may be modified as
 * desired for use in the WMtk project.
 *
 *****************************************************************************/

#ifndef NNET_MATH_H
#define NNET_MATH_H

#define EQUAL_TOLERANCE 1E-13

#include <string>

using namespace std;

void increment(int dim1, int dim2, double** matrix1, double** matrix2);

double multiply(int vector_size, double* vector);

void multiply(int vector_size, double* vector, int second_dimension, double** matrix, double* new_vector);

void transpose(int dim1, int dim2, double** matrix, double** new_matrix);

// For these the final argument can be the same object as the first two
void add(int size, double* vec1, double* vec2, double* new_vec);

void subtract(int size, double* vec1, double* vec2, double* new_vec);

void multiply_elements(int size, double* vec1, double* vec2, double* new_vec);

// Strange to put these here, but usefull debugging tools so what the hey...
void print_vector(int size, double* vec);
void print_matrix(int dim1, int dim2, double** matrix);

// Gibb's Softwmx Function
void applySoftmax(int size, double* vector, double* target_vector);
void applySoftmax(double scaling_parameter, int size, double* vector, double* target_vector);

int getIndexOfMaximum(int size, double* vector);

//Simple clip
double clip(double val, double min, double max);

// Number to string conversion routines - I guess they will work best in
// this header file?
string ftoa(double val);
string itoa(int val);

#endif
