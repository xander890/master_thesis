#ifndef ARRAYIMAGEBUFFER_H
#define ARRAYIMAGEBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>

class ArrayImageBuffer
{

public:
    GLuint arraytex;
    int size;
    int layers;
    /// Get rid of resources.
    void relinquish();


    /// Construct with given size
    ArrayImageBuffer(int size, int layerCount);

    /// Destroy
    ~ArrayImageBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable();


    Mesh::Texture * getColorTexture();
    void generateMipMaps();
    void enableMipMaps();
    void disableMipMaps();


    Mesh::Texture * colorTex;
};

#endif // ARRAYIMAGEBUFFER_H
