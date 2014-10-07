#include "arraytexturebuffer.h"

void ArrayTextureBuffer::relinquish()
{
    glDeleteTextures(1, &arraytex);
    glDeleteTextures(1, &depthtex);
    glDeleteFramebuffers(1, &fbo);
}

ArrayTextureBuffer::ArrayTextureBuffer(int size, int layerCount, int mipmaps, GLuint depth_view):
     depth_rb(0),
     arraytex(0),
     depthtex(0),
     fbo(0),
     size(size),
     layers(layerCount),
     levels(mipmaps)
{
    initialize(depth_view);
}

void ArrayTextureBuffer::initialize(GLuint depth_view)
{
    //relinquish();

    glGenFramebuffers(1,&fbo);

    glGenTextures(1, &arraytex);
    glGenTextures(1, &depthtex);

    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_LEVEL, 3);
    glTexStorage3D(	GL_TEXTURE_2D_ARRAY, levels, GL_RGBA32F, size, size, layers);

    //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);

    if(depth_view > 0)
    {
        glTextureView(depthtex, GL_TEXTURE_2D_ARRAY, depth_view, GL_DEPTH_COMPONENT32F, 0, 1, 0, layers);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, depthtex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

    if(depth_view == 0)
    {
        glTexStorage3D(	GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, size, size, layers);
    }


check_gl_error();
    check_gl_error();
    colorTex = new Mesh::Texture("colorMap", arraytex, GL_TEXTURE_2D_ARRAY);
    depthTex = new Mesh::Texture("depthMap",depthtex, GL_TEXTURE_2D_ARRAY);
    check_gl_error();
}

int ArrayTextureBuffer::enable(int layer)
{
    if(!(layer >= 0 && layer < layers))
    {
        std::cout << "Wrong level" << std::endl;
        return -1;
    }

    check_gl_error();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, arraytex, 0, layer);
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex, 0, layer);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    check_gl_error();

    return 0;
}

int ArrayTextureBuffer::enable()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, arraytex, 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    return 0;
}

Mesh::Texture * ArrayTextureBuffer::getColorTexture()
{
    return colorTex;
}

Mesh::Texture * ArrayTextureBuffer::getDepthTexture()
{
    return depthTex;
}

void ArrayTextureBuffer::generateMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ArrayTextureBuffer::enableMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ArrayTextureBuffer::disableMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
