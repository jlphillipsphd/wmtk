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

#include "hrrOperators.h"

// Overload the '+' operator for HRRs to be the linear addition of two vectors
HRR operator+(const HRR& hrr1, const HRR& hrr2) {
    if (hrr1.size() != hrr2.size()) {
        std::cerr << "Cannot add two HRRs of differing size!";
        return HRR();
    }

    HRR newHRR(hrr1.size());

    for ( int i = 0; i < hrr1.size(); i++ ) {
        newHRR[i] = hrr1[i] + hrr2[i];
    }

    return newHRR;
}

// Overload the '-' operator for HRRs to be the linear subtraction of two vectors
HRR operator-(const HRR& hrr1, const HRR& hrr2) {
    if (hrr1.size() != hrr2.size()) {
        std::cerr << "Cannot add two HRRs of differing size!";
        return HRR();
    }

    HRR newHRR(hrr1.size());

    for ( int i = 0; i < hrr1.size(); i++ ) {
        newHRR[i] = hrr1[i] - hrr2[i];
    }

    return newHRR;
}


// Overload the '*' operator for HRRs to be circular convolution of two vectors
HRR operator*(const HRR& hrr1, const HRR& hrr2) {
    HRR newConcept(hrr1.size());

	if (hrr1.size() != hrr2.size()) {
		// Notify user if attempted to convolve vectors of differing length
		std::cerr << "ERROR: Cannot convolve two hrrs of differing size!\n";
	} else {

		// Form the outer product of hrr1 and hrr2
		std::vector<HRR> outerProduct(hrr1.size());
		for (int i = 0; i < outerProduct.size(); i++){
			HRR newVector(hrr2.size());
			outerProduct[i] = newVector;
		}

		// Create the outerProduct matrix
		for (int i = 0; i < hrr1.size(); i ++) {
			for (int j = 0; j < hrr2.size(); j++) {
				outerProduct[i][j] = hrr1[i] * hrr2[j];
			}
		}

		// Perform circular convolution
		for (int j = 0; j < hrr1.size(); j++){
			double sum = 0.0;
			for (int i = j, k = 0; k <= j; i--, k++){
				sum += hrr1[i] * hrr2[k];

			}
			for (int i = hrr2.size()-1, k = j+1; k <= hrr2.size()-1; i--, k++){
				sum += hrr1[i] * hrr2[k];
			}
			newConcept[j] = sum;
		}

	}

	return newConcept;
}

// Overload the '*' operator for an HRR and a scalar
HRR operator*(const HRR& hrr, const double c) {
    HRR vector = hrr;
    for (double& d : vector) {
        d *= c;
    }
    return vector;
}
