#ifndef LIGHT_H
#define LIGHT_H


#include <CGLA/Vec4f.h>
#include <CGLA/Vec4f.h>

namespace GLGraphics
{

const static char* LIGHT_DIFFUSE_COLOR_UNIFORM = "light_diff";
const static char* LIGHT_SPEC_COLOR_UNIFORM = "light_spec";
const static char* LIGHT_POS_UNIFORM = "light_pos";

// a simple diffuse light
class Light
{
public:
    Light() : position(CGLA::Vec4f(0.0f)), isDirectional(true), diffuseColor(CGLA::Vec4f(1.0f)), intensity(1.0f), specularIntensity(CGLA::Vec4f(1.0f)) {}
    Light(CGLA::Vec4f pos, CGLA::Vec4f diffuse, float intensity, CGLA::Vec4f spec, bool isDirectional) :
        position(pos), isDirectional(isDirectional), diffuseColor(diffuse), specularIntensity(spec), intensity(intensity) {}

    CGLA::Vec4f position;
    bool isDirectional;
    CGLA::Vec4f diffuseColor;
    float intensity;
    CGLA::Vec4f specularIntensity;

};

}
#endif // LIGHT_H
