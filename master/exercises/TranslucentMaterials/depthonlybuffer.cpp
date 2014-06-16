#include "depthonlybuffer.h"


void DepthOnlyBuffer::relinquish()
{
    glDeleteTextures(1, &depthtex);
    glDeleteFramebuffers(1, &fbo);
}

DepthOnlyBuffer::DepthOnlyBuffer(int size, int layerCount) : depthtex(0), fbo(0), size(size), layers(layerCount)
{
    initialize();
}

void DepthOnlyBuffer::initialize()
{
    //relinquish();

    glGenFramebuffers(1,&fbo);
    glGenTextures(1, &depthtex);


    glBindTexture(GL_TEXTURE_2D_ARRAY, depthtex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glTexStorage3D(	GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32F, size, size, layers);


    depthTex = new Mesh::Texture("depthMap",depthtex, GL_TEXTURE_2D_ARRAY);

}

int DepthOnlyBuffer::enable(int layer)
{
    if(!(layer >= 0 && layer < layers))
    {
        std::cout << "Wrong level" << std::endl;
        return -1;
    }

    check_gl_error();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex, 0, layer);

    glDrawBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);

    return 0;
}

int DepthOnlyBuffer::enable()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex, 0);
    glDrawBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);
    return 0;
}


Mesh::Texture * DepthOnlyBuffer::getDepthTexture()
{
    return depthTex;
}

