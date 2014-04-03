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
    glTexImage2D(target, 0, GL_RGB32F , width, height,0, GL_RGB,GL_FLOAT, &data[0]);
    glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
}
