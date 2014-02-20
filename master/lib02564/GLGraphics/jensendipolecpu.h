#ifndef JENSENDIPOLECPU_H
#define JENSENDIPOLECPU_H
#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>

class JensenDipoleCPU
{
public:
    JensenDipoleCPU(Mesh::ScatteringMaterial m) : material(m) {}

    void calculate(std::vector<CGLA::Vec3f>& vertices, std::vector<CGLA::Vec3f>& normals, std::vector<CGLA::Vec4f>& resultColors);

private:
    Mesh::ScatteringMaterial material;
};

#endif // JENSENDIPOLECPU_H
