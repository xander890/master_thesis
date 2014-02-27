#ifndef JENSENDIPOLECPU_H
#define JENSENDIPOLECPU_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>

class DipoleCPU
{
public:
    Mesh::ScatteringMaterial material;
    GLGraphics::Light light;


    void calculate(std::vector<CGLA::Vec3f>& vertices, std::vector<CGLA::Vec3f>& normals, std::vector<CGLA::Vec4f>& resultColors);
    CGLA::Vec3f S_finite(CGLA::Vec3f xi,CGLA::Vec3f wi,CGLA::Vec3f xo, CGLA::Vec3f nin, CGLA::Vec3f no);
    CGLA::Vec3f S_infinite(CGLA::Vec3f x, CGLA::Vec3f w, float r, CGLA::Vec3f n_o);
    void calculate2x2Texture(float inclinationDegrees, std::vector<float> & texture, int textureSize);
private:
    CGLA::Vec3f invertVec3f(const CGLA::Vec3f vector);
    CGLA::Vec3f sqrtVec3f(const CGLA::Vec3f vector);
};

#endif // JENSENDIPOLECPU_H
