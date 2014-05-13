#ifndef IMAGETEXTURE_H
#define IMAGETEXTURE_H

#include<Mesh/texture.h>

namespace Mesh
{
class ImageTexture : public Mesh::Texture
{
public:
    ImageTexture(const std::string& _name, GLint id, GLenum target, GLenum access) : Mesh::Texture(_name,id,target), access(access)
    {}



    void bindImage(int activeImage);
    virtual void init() {}

private:

    GLenum access;
};
}

#endif // IMAGETEXTURE_H
