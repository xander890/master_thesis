#include "MipMapGenerator.h"

void MipMapGenerator::relinquish()
{
    glDeleteTextures(1, &arraytex);
    glDeleteFramebuffers(1, &fbo);
}

MipMapGenerator::MipMapGenerator(int size, int layers) : arraytex(0), fbo(0), size(size), layers(layers)
{

    initialize();
}

void MipMapGenerator::initialize()
{
    //relinquish();

    glGenFramebuffers(1,&fbo);

    glGenTextures(1, &arraytex);

    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, levels, GL_RGBA16F, size, size, layers);
    //Upload pixel data.
    //The first 0 refers to the mipmap level (level 0, since there's only 1)
    //The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
    //The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
    //Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.

    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA16F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);
    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Something wrong with FBO" << std::endl;
    colorTex = new Mesh::Texture("colorMapX", arraytex, GL_TEXTURE_2D_ARRAY);
}

int MipMapGenerator::enable(int level)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, arraytex, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClear(GL_COLOR_BUFFER_BIT);
    return 0;
}

Mesh::Texture *MipMapGenerator::getColorTexture()
{
    return colorTex;
}

