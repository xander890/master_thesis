#ifndef BETTERDIPOLE_H
#define BETTERDIPOLE_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <Dipoles/bssrdf.h>

class BetterDipole : public BSSRDF
{
public:
    BetterDipole::BetterDipole(Mesh::ScatteringMaterial & m): BSSRDF(m) {}
    BetterDipole::BetterDipole(Mesh::ScatteringMaterial & m, SingleScattering & s): BSSRDF(m,s) {}

    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & wo, const CGLA::Vec3f & no);
};

#endif // BETTERDIPOLE_H
