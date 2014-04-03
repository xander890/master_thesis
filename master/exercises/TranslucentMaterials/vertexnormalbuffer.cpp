#include "vertexnormalbuffer.h"

using namespace std;


void VertexNormalBuffer::relinquish()
{
    glDeleteTextures(1, &vtex);
    glDeleteTextures(1, &ntex);
    glDeleteRenderbuffers(1, &rb);
    glDeleteFramebuffers(1, &fbo);
}

VertexNormalBuffer::VertexNormalBuffer(int size) : fbo(0), rb(0), vtex(0), ntex(0), size(size)
{
    initialize();
}

void VertexNormalBuffer::initialize()
{
    relinquish();

    glGenTextures(1, &vtex);
    glBindTexture(GL_TEXTURE_2D, vtex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size, size, 0, GL_RGBA, GL_FLOAT, 0);

    glGenTextures(1, &ntex);
    glBindTexture(GL_TEXTURE_2D, ntex);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, size, size, 0, GL_RGBA, GL_FLOAT, 0);

    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vtex, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ntex, 0);

    glGenRenderbuffers(1,&rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size, size);

    glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb);

    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "Something wrong with FBO" << endl;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

int VertexNormalBuffer::enable()
{
    GLint old_draw_buffer;
    glGetIntegerv(GL_DRAW_BUFFER, &old_draw_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    GLenum buffers[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);
    glClearColor(0,0,-1000.0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    return old_draw_buffer;
}

/*
void VertexNormalBuffer::bind_textures(int vertex_unit, int normal_unit)
{
    glActiveTexture(GL_TEXTURE0+vertex_unit);
    glBindTexture(GL_TEXTURE_2D, vtex);
    glActiveTexture(GL_TEXTURE0+normal_unit);
    glBindTexture(GL_TEXTURE_2D, ntex);
}*/

Mesh::Texture * VertexNormalBuffer::getVertexTexture()
{
    return new Mesh::Texture("vtex",vtex,GL_TEXTURE_2D);
}

Mesh::Texture * VertexNormalBuffer::getNormalTexture()
{
    return new Mesh::Texture("ntex",ntex,GL_TEXTURE_2D);
}
