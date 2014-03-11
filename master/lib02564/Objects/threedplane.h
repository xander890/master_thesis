#ifndef THREEDPLANE_H
#define THREEDPLANE_H
#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"

namespace GLGraphics
{
class ThreeDPlane : public ThreeDObject
{

public:
    ThreeDPlane() : tex(0), ThreeDObject()
    {
    }
    virtual void display(ShaderProgramDraw& shader) override;
    virtual bool init(std::string filename, std::string name, Mesh::Material & material) override;
    void setTexture(const std::vector<CGLA::Vec3f> & data, const int size);

private:
    GLuint tex;
};
}
#endif // THREEDPLANE_H
