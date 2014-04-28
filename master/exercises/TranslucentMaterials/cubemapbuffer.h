#ifndef CUBEMAPBUFFER_H
#define CUBEMAPBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>

class CubemapBuffer
{
    GLuint fbo; // The framebuffer object and the render buffer
    GLuint cubetex, depthtex;
    int size;
    /// Get rid of resources.
    void relinquish();

public:

    /// Construct with given size
    CubemapBuffer(int size);

    /// Destroy
    ~CubemapBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int face);

    Mesh::Texture * getCubemapTexture();
    Mesh::Texture * getCubemapDepthTexture();
    void generateMipMaps();

};

#endif // CUBEMAPBUFFER_H
