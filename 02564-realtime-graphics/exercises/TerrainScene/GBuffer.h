/*
 *  GBuffer.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include <GLGraphics/GLHeader.h>

/** A GBuffer is used in deferred rendering. It captures information about eye space 
 position and normal direction for each pixel. Consequently, we can use the GBuffer to 
 capture all the information we need to later shade the scene. 
 Essentially, the GBuffer is a wrapper round a geometry texture (eye space positions)
 a normal texture and a color texture. These textures are rendered to in one go using
 multiple render targets. The GBuffer class sets up an FBO so that you can render to these
 three textures. However, the shader program needed could depend on the object
 being rendered so the program for drawing to a GBuffer is outside the classe. */
class GBuffer
{
	GLuint fbo, rb; // The framebuffer object and the render buffer
	GLuint gtex, ntex, ctex; // The three textures needed: geometry, normal, color 
    int width;
    int height;
	/// Get rid of resources. 
	void relinquish();

public:
	
	/// Construct with given size
    GBuffer(int width, int height);
	
	/// Destroy
	~GBuffer() {relinquish();}
	
    /**
     * Rebuilds the gbuffer when width and height is resized
     */
    void rebuild_on_resize(int width, int height);

	/** Initialize with width W and height H. Call if, for instance, the window changes
	 size and a different size GBuffer is needed. */
	void initialize(int W, int H);
	
	/** Enable rendering to the FBO that is associated with the GBuffer. No enable since we 
	 might not want to switch back to the previous framebuffer. */
	int enable();
	
	/// Bind the three GBuffer textures to the units passed as arguments.
	void bind_textures(int gtex_unit, int ntex_unit, int ctex_unit);
};

#endif
