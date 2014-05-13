#ifndef DEPTHONLYBUFFER_H
#define DEPTHONLYBUFFER_H

#include <GLGraphics/GLHeader.h>
#include <Mesh/texture.h>


class DepthOnlyBuffer
{

public:
    GLuint fbo; // The framebuffer object and the render buffer
    GLuint depthtex;
    int size;
    int layers;
    /// Get rid of resources.
    void relinquish();


    /// Construct with given size
    DepthOnlyBuffer(int size, int layerCount);

    /// Destroy
    ~DepthOnlyBuffer() {relinquish();}

    void initialize();

    /** Enable rendering to the FBO that is associated with the GBuffer. No enable since we
     might not want to switch back to the previous framebuffer. */
    int enable(int level);
    int enable();

    Mesh::Texture * getDepthTexture();

    Mesh::Texture * depthTex;
};

#endif // DEPTHONLYBUFFER_H
