#include "mipmapgeneratorview.h"

using namespace std;
using namespace Mesh;

void MipMapGeneratorView::relinquish()
{

    glDeleteTextures(mipmaps, &(arraytex->at(0)));
    glDeleteTextures(mipmaps, &(depthtex->at(0)));
    glDeleteFramebuffers(mipmaps, &(fbo->at(0)));

}

MipMapGeneratorView::MipMapGeneratorView(GLuint originalColorTexture, GLuint originalDepthTexture, int originalSize, int layerCount, int mipmaps) :
    originalColorTexture(originalColorTexture),
    originalDepthTexture(originalDepthTexture),
    fbo(new vector<GLuint>(mipmaps)),
    originalSize(originalSize),
    layers(layerCount),
    mipmaps(mipmaps),
    arraytex(new vector<GLuint>(mipmaps)),
    depthtex(new vector<GLuint>(mipmaps)),
    colorTex(new vector<Texture*>(mipmaps)),
    depthTex(new vector<Texture*>(mipmaps))
{
    initialize();
}

void MipMapGeneratorView::initialize()
{
    //relinquish();

    for(int i = 0; i < mipmaps; i++)
    {
        glGenFramebuffers(1,&(fbo->at(i)));
        glGenTextures(1, &(arraytex->at(i)));
        glGenTextures(1, &(depthtex->at(i)));

        int size = originalSize / (2 << i);


        glTextureView(	arraytex->at(i), GL_TEXTURE_2D_ARRAY, originalColorTexture, GL_RGBA32F, i + 1, 1, 0, layers);
        //glTextureView(	depthtex->at(i), GL_TEXTURE_2D_ARRAY, originalDepthTexture, GL_DEPTH_COMPONENT32, i + 1, 1, 0, layers);

        //glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);
        glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex->at(i));
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glBindTexture(GL_TEXTURE_2D_ARRAY, depthtex->at(i));
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, size, size, layers, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        check_gl_error();
        colorTex->at(i) = new Mesh::Texture("mipcolorMap", arraytex->at(i), GL_TEXTURE_2D_ARRAY);
        depthTex->at(i) = new Mesh::Texture("mipdepthMap",depthtex->at(i), GL_TEXTURE_2D_ARRAY);
    }

}

int MipMapGeneratorView::enable(int layer, int level)
{
    if(!(layer >= 0 && layer < layers) || !(level >= 0 && level < mipmaps))
    {
        std::cout << "Wrong level or layer" << std::endl;
        return -1;
    }

    check_gl_error();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->at(level));
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, arraytex->at(level), 0, layer);
    glFramebufferTextureLayer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex->at(level), 0, layer);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    return 0;
}

int MipMapGeneratorView::enableUniqueTarget(int level)
{
    if(!(level >= 0 && level < mipmaps))
        return -1;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->at(level));
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, arraytex->at(level), 0);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthtex->at(level), 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    return 0;
}



Mesh::Texture * MipMapGeneratorView::getColorTexture(int level)
{
    if(level >= 0 && level < mipmaps)
        return colorTex->at(level);
    return nullptr;
}

Mesh::Texture * MipMapGeneratorView::getDepthTexture(int level)
{
    if(level >= 0 && level < mipmaps)
        return depthTex->at(level);
    return nullptr;
}

GLuint MipMapGeneratorView::getFBO(int level)
{
    if(level >= 0 && level < mipmaps)
        return fbo->at(level);
    return -1;
}

