#ifndef DIRECTIONALDIPOLE_H
#define DIRECTIONALDIPOLE_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>
#include <GLGraphics/light.h>
#include <Mesh/TriangleMesh.h>
#include <GLGraphics/ThreeDObject.h>
#include <Dipoles/bssrdf.h>

class DirectionalDipole : public BSSRDF
{
public:
    DirectionalDipole::DirectionalDipole(Mesh::ScatteringMaterial mat) : BSSRDF(mat) {}

    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & no);

    CGLA::Vec3f S_finite(CGLA::Vec3f xi,CGLA::Vec3f wi,CGLA::Vec3f xo, CGLA::Vec3f nin, CGLA::Vec3f no);
    CGLA::Vec3f S_infinite(CGLA::Vec3f x, CGLA::Vec3f w, CGLA::Vec3f r, CGLA::Vec3f n_o);
    void calculate2x2Texture(float inclinationDegrees, std::vector<CGLA::Vec3f> & texture, int textureSize);
};

#endif // DIRECTIONALDIPOLE_H
