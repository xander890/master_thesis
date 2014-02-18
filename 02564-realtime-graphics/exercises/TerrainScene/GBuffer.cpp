/*
 *  GBuffer.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <iostream>
#include "GBuffer.h"

using namespace std;
void GBuffer::relinquish()
{
    glDeleteTextures(1, &gtex);
    glDeleteTextures(1, &ntex);
    glDeleteTextures(1, &ctex);
	glDeleteRenderbuffers(1, &rb);
	glDeleteFramebuffers(1, &fbo);
}

void GBuffer::rebuild_on_resize(int width, int height)
{
    if (this->width != width || this->height != height){
        initialize(width, height);
    }
}

void GBuffer::initialize(int WINDOW_WIDTH, int WINDOW_HEIGHT)
{
    width = WINDOW_WIDTH;
    height = WINDOW_HEIGHT;
	relinquish();
	
    glGenTextures(1, &gtex);
    glBindTexture(GL_TEXTURE_RECTANGLE, gtex);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	
    glGenTextures(1, &ntex);
    glBindTexture(GL_TEXTURE_RECTANGLE, ntex);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	
    glGenTextures(1, &ctex);
    glBindTexture(GL_TEXTURE_RECTANGLE, ctex);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, 0);
	
	
    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gtex, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, ntex, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_RECTANGLE, ctex, 0);
	
    glGenRenderbuffers(1,&rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WINDOW_WIDTH, WINDOW_HEIGHT);
	
    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);
	
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Something wrong with FBO" << endl;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

GBuffer::GBuffer(int WINDOW_WIDTH, int WINDOW_HEIGHT): fbo(0), rb(0), gtex(0), ntex(0), ctex(0)
{
	initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
}


void GBuffer::bind_textures(int gtex_unit, int ntex_unit, int ctex_unit)
{
    glActiveTexture(GL_TEXTURE0+gtex_unit);
    glBindTexture(GL_TEXTURE_RECTANGLE, gtex);
    glActiveTexture(GL_TEXTURE0+ntex_unit);
    glBindTexture(GL_TEXTURE_RECTANGLE, ntex);
    glActiveTexture(GL_TEXTURE0+ctex_unit);
    glBindTexture(GL_TEXTURE_RECTANGLE, ctex);
}

GLint GBuffer::enable()
{
	GLint old_draw_buffer;
	glGetIntegerv(GL_DRAW_BUFFER, &old_draw_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	
    GLenum buffers[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, buffers);
	glClearColor(0,0,-1000.0,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	
	return old_draw_buffer;
}

