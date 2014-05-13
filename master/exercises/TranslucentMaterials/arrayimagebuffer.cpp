#include "arrayimagebuffer.h"

void ArrayImageBuffer::relinquish()
{
    glDeleteTextures(1, &arraytex);
}

ArrayImageBuffer::ArrayImageBuffer(int size, int layerCount) : arraytex(0), size(size), layers(layerCount)
{

    initialize();
}

void ArrayImageBuffer::initialize()
{
    //relinquish();


    glGenTextures(1, &arraytex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAX_LEVEL, 3);

    //glTexStorage3D(GL_TEXTURE_2D_ARRAY, levels, GL_RGBA16F, size, size, layers);
    //Upload pixel data.
    //The first 0 refers to the mipmap level (level 0, since there's only 1)
    //The following 2 zeroes refers to the x and y offsets in case you only want to specify a subrectangle.
    //The final 0 refers to the layer index offset (we start from index 0 and have 2 levels).
    //Altogether you can specify a 3D box subset of the overall texture, but only one mip level at a time.


    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA32F, size, size, layers, 0, GL_RGBA, GL_FLOAT, 0);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glBindImageTexture(0, arraytex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

    colorTex = new Mesh::Texture("colorMap", arraytex, GL_TEXTURE_2D_ARRAY);
}


int ArrayImageBuffer::enable()
{
   return 0;
}

Mesh::Texture * ArrayImageBuffer::getColorTexture()
{
    return colorTex;
}


void ArrayImageBuffer::generateMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ArrayImageBuffer::enableMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void ArrayImageBuffer::disableMipMaps()
{
    glBindTexture(GL_TEXTURE_2D_ARRAY, arraytex);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
