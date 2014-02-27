#ifndef THREEDPLANE_H
#define THREEDPLANE_H
#include "GLGraphics/ThreeDObject.h"
#include "GLGraphics/light.h"

const int TEXTURE_SIZE = 256;

namespace GLGraphics
{
class ThreeDPlane : public ThreeDObject
{

public:
    ThreeDPlane(Light l) : tex(0), light(l), ThreeDObject()
    {
    }
    virtual void display(ShaderProgramDraw& shader) override;
    virtual bool init(std::string filename, std::string name) override;

private:
    GLuint tex;
    Light light;
};
}
#endif // THREEDPLANE_H
