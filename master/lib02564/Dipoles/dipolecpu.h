#ifndef JENSENDIPOLECPU_H
#define JENSENDIPOLECPU_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include "Dipoles/bssrdf.h"

class DipoleCPU
{
public:
    GLGraphics::Light light;
    void calculate(GLGraphics::ThreeDObject & three, std::vector<CGLA::Vec3f>& outLuminance, BSSRDF & bssrf);
};

#endif // JENSENDIPOLECPU_H
