#include "texture.h"

using namespace std;

void Mesh::Texture::init()
{
    if(id != 0)
    {
        // The texture is already initialized or loaded elsewhere via a fbo. Not initializing again.
        return;
    }

    glGenTextures(1,&id);
    glBindTexture(target, id);
    glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
    load();
}

void Mesh::Texture::reloadData(std::vector<CGLA::Vec3f> & data, int newWidth, int newHeight)
{
    width = newWidth;
    height = newHeight;
    this->data = data;
    load();
}

std::vector<CGLA::Vec3f> Mesh::Texture::getData()
{
    return data;
}

void Mesh::Texture::load()
{
    glTexImage2D(target, 0, GL_RGB32F, width, height,0, GL_RGB,GL_FLOAT, &data[0]);
}
