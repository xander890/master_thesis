#include "cubemapbuffer.h"


void CubemapBuffer::relinquish()
{
    glDeleteTextures(1, &cubetex);
    glDeleteTextures(1, &depthtex);
    glDeleteFramebuffers(1, &fbo);
}

CubemapBuffer::CubemapBuffer(int size) : cubetex(0), fbo(0), depthtex(0),size(size)
{
    initialize();
}

void CubemapBuffer::initialize()
{
    relinquish();

    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for(int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, size, size, 0, GL_RGBA, GL_FLOAT, 0);
    }

    glGenTextures(1, &depthtex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthtex);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    for(int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, size, size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    glGenFramebuffers(1,&fbo);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vtex, 0);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ntex, 0);

//    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        cout << "Something wrong with FBO" << endl;
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

}

int CubemapBuffer::enable(int face)
{
    if(face < 0 || face > 5)
    {
        std::cout << "Wrong face passed to Cubemap buffer" << std::endl;
        return -1;
    }

    check_gl_error();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, cubetex, 0);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, depthtex, 0);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    check_gl_error();

    return 0;
}

Mesh::Texture * CubemapBuffer::getCubemapTexture()
{
    return new Mesh::Texture("colorCubemap", cubetex, GL_TEXTURE_CUBE_MAP);
}

Mesh::Texture * CubemapBuffer::getCubemapDepthTexture()
{
    return new Mesh::Texture("depthCubemap",depthtex, GL_TEXTURE_CUBE_MAP);
}
