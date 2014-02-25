#ifndef JENSENDIPOLECPU_H
#define JENSENDIPOLECPU_H
#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
class JensenDipoleCPU
{
public:
    Mesh::ScatteringMaterial material;
    GLGraphics::Light light;


    void calculate(std::vector<CGLA::Vec3f>& vertices, std::vector<CGLA::Vec3f>& normals, std::vector<CGLA::Vec4f>& resultColors);


};

#endif // JENSENDIPOLECPU_H
