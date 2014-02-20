/*
 *  aux.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLGraphics/GLHeader.h>
#include "auxil.h"
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <cmath>
#include <vector>

using namespace std;
using namespace CGLA;


double HaltonNumber(int index, int base)
{
	double result = 0.0;
	double f = 1.0 / base;
	int i = index;
	while(i>0)
	{
		result += f * (i%base);
		i = floor(i/double(base));
		f = f/base;
	}
	return result;
}

float noise(float x, float y)
{
	static const int NSINES = 40;
	static vector<Vec2f> freqs(NSINES);
	static bool was_here = false;
	if(!was_here)
	{
        gel_srand(0);
        was_here = true;
		for(int i=0;i<NSINES;++i)
		{
			Vec2f v(0);
			float l;
			do {
                v = 2.0f*Vec2f(gel_rand(), gel_rand())/GEL_RAND_MAX-Vec2f(1);
				l = length(v);
			}
            while(l<0.5f || l>1.0f);
			freqs[i] = v;
		}
	}
	float h=0;
	for(int i=0;i<NSINES;++i)
		h += sin(dot(Vec2f(x,y), freqs[i]));
    return (1.0f/NSINES)*h;
}

float turbulence(float x, float y)
{
	float scale=1.0;
	float t=0;
	for(int i=0;i<10;++i,scale *= 2.0)
		t+=abs(noise(x*scale,y*scale))/scale;
	return t;
}
