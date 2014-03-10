#ifndef JENSENDIPOLE_H
#define JENSENDIPOLE_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <Dipoles/bssrdf.h>

class JensenDipole : public BSSRDF
{
public:
    JensenDipole::JensenDipole(Mesh::ScatteringMaterial mat) : BSSRDF(mat) {}

    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & wo, const CGLA::Vec3f & no);
};


#endif // JENSENDIPOLE_H
