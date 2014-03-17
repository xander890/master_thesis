#ifndef DIPOLEGPU_H
#define DIPOLEGPU_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include "Dipoles/bssrdf.h"
#include "GLGraphics/User.h"

class DipoleGPU
{
public:
    void prepare(GLGraphics::ThreeDObject & three);
};

#endif // DIPOLEGPU_H
