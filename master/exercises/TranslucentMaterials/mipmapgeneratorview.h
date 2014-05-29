#ifndef MIPMAPGENERATORVIEW_H
#define MIPMAPGENERATORVIEW_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>

class MipMapGeneratorView
{
    std::vector<GLuint> * fbo; // The framebuffer object and the render buffer
    std::vector<GLuint> * arraytex;
    std::vector<GLuint> * depthtex;
    int originalSize;
    int layers;
    int mipmaps;
    GLuint originalColorTexture;
    GLuint originalDepthTexture;
    /// Get rid of resources.
    void relinquish();

public:
        /// Construct with given size
    MipMapGeneratorView(GLuint originalColorTexture, GLuint originalDepthTexture, int originalSize, int layerCount = 6, int mipmaps = 1);

    /// Destroy
    ~MipMapGeneratorView() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int layer, int level);
    int enableUniqueTarget(int level);

    Mesh::Texture * getColorTexture(int level);
    Mesh::Texture * getDepthTexture(int level);

    std::vector<Mesh::Texture*> * colorTex;
    std::vector<Mesh::Texture*> * depthTex;

    GLuint getFBO(int level);

};

#endif // MIPMAPGENERATORVIEW_H
