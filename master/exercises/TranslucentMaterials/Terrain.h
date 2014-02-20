/*
 *  terrain.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <CGLA/Vec3f.h>
#include <GLGraphics/User.h>
#include <GLGraphics/ShaderProgram.h>

/** This class represents a terrain. It is very simple: The terrain is synthesized using noise
 and once created we can query height and normal at a given 2D location or render the thing. */
class Terrain: public GLGraphics::HeightMap
{
	float A;
	float delta;
	
	float x0, y0;
	
public:
	
	/// Construct terrain with amplitude A and scaling delta.
    Terrain(float _A, float _delta): A(_A), delta(_delta), x0(12.231f), y0(7.91f) {}
	
	/// Returns height at given 2D location
	float height(float _x, float _y) const;
	
	/// Returns normal at given 2D location
	CGLA::Vec3f normal(float x, float y) const;
	
	/// Draw using OpenGL: Assumes a reasonable shading program is used.
	void draw(GLGraphics::ShaderProgramDraw&);
};

#endif
