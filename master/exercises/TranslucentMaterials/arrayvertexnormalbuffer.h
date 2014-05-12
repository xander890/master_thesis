#ifndef ARRAYVERTEXNORMALBUFFER_H
#define ARRAYVERTEXNORMALBUFFER_H


#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>

class ArrayVertexNormalBuffer
{
    GLuint fbo, rb; // The framebuffer object and the render buffer
    GLuint vtex, ntex; // The two textures needed: geometry, normal,
    GLuint dtex;
    int size;
    int layers;
    /// Get rid of resources.
    void relinquish();
    Mesh::Texture * vtex_tex;
    Mesh::Texture * ntex_tex;

public:

    /// Construct with given size
    ArrayVertexNormalBuffer(int size, int layers);

    /// Destroy
    ~ArrayVertexNormalBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable();

    /// Bind the three GBuffer textures to the units passed as arguments.
    //void bind_textures(int vertex_unit, int normal_unit);

    Mesh::Texture * getVertexTexture();
    Mesh::Texture * getNormalTexture();
};
#endif // ARRAYVERTEXNORMALBUFFER_H
