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

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, levels, GL_DEPTH_COMPONENT32, size, size, layers);
    //Upload pixel data.
    //The first 0 refers to the mipmap level (level 0, since there's only 1)
    //The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
    //The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
    //Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, size, size, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);


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

