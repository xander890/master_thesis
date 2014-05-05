#ifndef MIPMAPGENERATOR_H
#define MIPMAPGENERATOR_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>


class MipMapGenerator
{

    GLuint fbo; // The framebuffer object and the render buffer
    GLuint arraytex;
    int size, layers;
    /// Get rid of resources.
    void relinquish();

public:

    /// Construct with given size
    MipMapGenerator(int size, int layers);

    /// Destroy
    ~MipMapGenerator() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int mipMaplevel);

    Mesh::Texture * colorTex;
    Mesh::Texture * getColorTexture();
};


#endif // MIPMAPGENERATOR_H
