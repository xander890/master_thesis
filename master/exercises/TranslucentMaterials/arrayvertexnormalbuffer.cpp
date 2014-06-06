#include "arrayvertexnormalbuffer.h"

using namespace std;


void ArrayVertexNormalBuffer::relinquish()
{
    glDeleteTextures(1, &vtex);
    glDeleteTextures(1, &ntex);
    glDeleteTextures(1, &dtex);
    glDeleteFramebuffers(1, &fbo);
}

ArrayVertexNormalBuffer::ArrayVertexNormalBuffer(int size, int layers) : fbo(0), rb(0), vtex(0), ntex(0), dtex(0), size(size), layers(layers)
{
    initialize();
}

void ArrayVertexNormalBuffer::initialize()
{
    //relinquish();

    glGenFramebuffers(1,&fbo);
    glGenTextures(1, &vtex);
    glGenTextures(1, &ntex);
    glGenTextures(1, &dtex);

    glBindTexture(GL_TEXTURE_2D_ARRAY, vtex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D_ARRAY, ntex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);



    glBindTexture(GL_TEXTURE_2D_ARRAY, dtex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, size, size, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    check_gl_error();



    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    ntex_tex = new Mesh::Texture("ntex",ntex,GL_TEXTURE_2D_ARRAY);
    vtex_tex = new Mesh::Texture("vtex",vtex,GL_TEXTURE_2D_ARRAY);
}

int ArrayVertexNormalBuffer::enable()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, vtex, 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, ntex, 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dtex, 0);

    const GLenum buffers[] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, buffers);
    glClearColor(0,0,-1000.0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

/*
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "Something wrong with FBO" << endl;
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        cout << "Gotcha!1" << endl;
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)
        cout << "Gotcha!2" << endl;
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        cout << "Gotcha!3" << endl;
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_UNSUPPORTED)
        cout << "Gotcha!4" << endl;
*/
    return 0;

}


Mesh::Texture * ArrayVertexNormalBuffer::getVertexTexture()
{
    return vtex_tex;
}

Mesh::Texture * ArrayVertexNormalBuffer::getNormalTexture()
{
    return ntex_tex;
}
