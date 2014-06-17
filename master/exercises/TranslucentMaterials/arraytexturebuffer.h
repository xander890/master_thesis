#ifndef ARRAYTEXTUREBUFFER_H
#define ARRAYTEXTUREBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>


class ArrayTextureBuffer
{

public:
    GLuint fbo,depth_rb; // The framebuffer object and the render buffer
    GLuint arraytex,depthtex;
    int size;
    int layers;
    int levels;
    /// Get rid of resources.
    void relinquish();


    /// Construct with given size
    ArrayTextureBuffer(int size, int layerCount, int mipmaps = 1, GLuint depth_view = 0);
    /// Destroy
    ~ArrayTextureBuffer() {relinquish();}

    void initialize(GLuint depth_view);

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int level);
    int enable();

    Mesh::Texture * getColorTexture();
    Mesh::Texture * getDepthTexture();
    void generateMipMaps();
    void enableMipMaps();
    void disableMipMaps();


    Mesh::Texture * colorTex;
    Mesh::Texture * depthTex;
};

#endif // ARRAYTEXTUREBUFFER_H
