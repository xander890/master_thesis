#ifndef TEXTUREBUFFER_H
#define TEXTUREBUFFER_H

#include <Mesh/texture.h>

namespace Mesh
{
template <class T>
class TextureBuffer : public Mesh::Texture
{
public:
    TextureBuffer(const std::string& _name, unsigned int width, GLenum internalFormat, std::vector<T> & data):
        Mesh::Texture(_name,GL_TEXTURE_BUFFER, width, width, std::vector<CGLA::Vec3f>(1)),
        bufferdata(data),
        internalFormat(internalFormat),
        tbo(0)
    {}


    /**
     * Use this constructor for a texture generated via a buffer or a fbo, so no data needs to be loaded.
     * So the texture wraps the data that can be sent to shader via a material.
     */
    TextureBuffer(const std::string& _name, GLint id): Texture(_name, id, GL_TEXTURE_BUFFER)
    {}

    // Initializes the texture wrt OpenGL.

    virtual void init()
    {
        glGenBuffers(1, &tbo);
        glBindBuffer(GL_TEXTURE_BUFFER, tbo);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(T) * bufferdata.size(), &(bufferdata[0]), GL_STATIC_DRAW);

        glGenTextures(1, &id);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    // binds the data of the texutre
    virtual void bind()
    {
        Texture::bind();
        glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, tbo);
    }

    void reloadBufferData(std::vector<T> newData)
    {
        bufferdata = newData;
        glBindBuffer(GL_TEXTURE_BUFFER, tbo);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(T) * bufferdata.size(), &(bufferdata[0]), GL_STATIC_DRAW);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

private:
    std::vector<T> bufferdata;
    GLuint tbo;
    GLenum internalFormat;
};
}
#endif // TEXTUREBUFFER_H
