#ifndef MIPMAPGENERATOR_H
#define MIPMAPGENERATOR_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>


class MipMapGenerator
{

    GLuint fbo,depth_rb; // The framebuffer object and the render buffer
    GLuint arraytex,depthtex;
    int size;
    int layers;
    int levels;
    /// Get rid of resources.
    void relinquish();

public:
        /// Construct with given size
    MipMapGenerator(int size, int layerCount = 6, int mipmaps = 1);

    /// Destroy
    ~MipMapGenerator() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int level);
    int enableUniqueTarget();

    Mesh::Texture * getColorTexture();
    Mesh::Texture * getDepthTexture();
    void generateMipMaps();

    Mesh::Texture * colorTex;
    Mesh::Texture * depthTex;

    GLuint getFBO() {return fbo;}

};


#endif // MIPMAPGENERATOR_H
