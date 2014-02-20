/*
 *  aux.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __AUX_H__
#define __AUX_H__

#include <CGLA/Vec3f.h>

/// Noise function based on something like inverse fourier transform (not quite but close).
float noise(float x, float y);

/// Add many octaves of noise.
float turbulence(float x, float y);

/// Generate a quasi random number according to the Halton sequence of base base.
double HaltonNumber(int index, int base);


#endif