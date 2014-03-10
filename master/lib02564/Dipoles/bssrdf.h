#ifndef BSSRDF_H
#define BSSRDF_H

#include "CGLA/Vec3f.h"
#include "Mesh/scatteringmaterial.h"

class BSSRDF
{

public:
    BSSRDF::BSSRDF(Mesh::ScatteringMaterial & m): material(m) {}
    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & wo, const CGLA::Vec3f & no)  {return CGLA::Vec3f(0.0f);}
    Mesh::ScatteringMaterial material;


};

#endif // BSSRDF_H
