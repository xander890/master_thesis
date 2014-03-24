/*
 *  ShadowBuffer.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SHADOWBUFFER_H__
#define __SHADOWBUFFER_H__

#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ShaderProgram.h>
#include <CGLA/Mat4x4f.h>

const static char * SHADOW_TEX_ID = "shadow";
const static char * SHADOW_MATRIX_ID = "Mat";
const static char * SHADOW_ENABLED = "shadowEnabled";

/** This class represents a shadow buffer. Essentially, this is just a texture
 and a framebuffer object wrapped in a class. Note that the shadow is a depth 
 texture */
class ShadowBuffer
{
	const int dim; // Size
	GLuint fbo, rb; // The framebuffer object and the render buffer
	GLuint dtex; // The depth texture 
    CGLA::Mat4x4f mat; //The space conversion matrix
	
	/// Let go of resources 
	void relinquish();
public:
	
	/// Construct with default size 1024
    ShadowBuffer(int _dim = 1024);
	
	/// Destroy, releasing resources
	~ShadowBuffer() {relinquish();}

	/// Initialize: Acquire resources. 
	void initialize();
	
	/** Enable the framebuffer object. There is no disable(). This seems unsymmetrical, but 
	 we do not necessarily switch back to the framebuffer we had prior to enable. So switching 
	 back might incur an unnecessary FBO switch. */
	int enable();
		
	/// Bind the depth map texture to specified tex unit.
	void bind_textures(int dtex_unit);

    CGLA::Mat4x4f getLightWorldTransformationMatrix();
    void setLightWorldTransformationMatrix(CGLA::Mat4x4f & matrix);
};

#endif
