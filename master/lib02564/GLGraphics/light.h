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
    Light() : position(CGLA::Vec4f(0.0f)), isDirectional(true), diffuseIntensity(CGLA::Vec4f(1.0f)), specularIntensity(CGLA::Vec4f(1.0f)) {}
    Light(CGLA::Vec4f pos, CGLA::Vec4f diffuse, CGLA::Vec4f spec, bool isDirectional) :
        position(pos), isDirectional(isDirectional), diffuseIntensity(diffuse), specularIntensity(spec) { std::cout << pos;}

    CGLA::Vec4f position;
    bool isDirectional;
    CGLA::Vec4f diffuseIntensity;
    CGLA::Vec4f specularIntensity;

};

}
#endif // LIGHT_H
