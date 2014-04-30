#ifndef ARRAYTEXTUREBUFFER_H
#define ARRAYTEXTUREBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>


class ArrayTextureBuffer
{

    GLuint fbo; // The framebuffer object and the render buffer
    GLuint arraytex,depthtex;
    int size;
    int layers;
    int levels;
    /// Get rid of resources.
    void relinquish();

public:

    /// Construct with given size
    ArrayTextureBuffer(int size, int layerCount = 6, int mipmaps = 1);

    /// Destroy
    ~ArrayTextureBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int level);

    Mesh::Texture * getColorTexture();
    Mesh::Texture * getDepthTexture();
    void generateMipMaps();

    Mesh::Texture * colorTex;
    Mesh::Texture * depthTex;
};

#endif // ARRAYTEXTUREBUFFER_H
