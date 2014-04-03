#ifndef VERTEXNORMALBUFFER_H
#define VERTEXNORMALBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>

class VertexNormalBuffer
{
    GLuint fbo, rb; // The framebuffer object and the render buffer
    GLuint vtex, ntex; // The three textures needed: geometry, normal, color
    int size;
    /// Get rid of resources.
    void relinquish();

public:

    /// Construct with given size
    VertexNormalBuffer(int size);

    /// Destroy
    ~VertexNormalBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable();

    /// Bind the three GBuffer textures to the units passed as arguments.
    //void bind_textures(int vertex_unit, int normal_unit);

    Mesh::Texture * getVertexTexture();
    Mesh::Texture * getNormalTexture();
};

#endif // VERTEXNORMALBUFFER_H
