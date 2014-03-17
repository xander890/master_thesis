#ifndef BSSRDF_H
#define BSSRDF_H

#include "CGLA/Vec3f.h"
#include "Mesh/scatteringmaterial.h"
#include "singlescattering.h"

class BSSRDF
{

public:
    BSSRDF::BSSRDF() : material(Mesh::ScatteringMaterial()), has_single_scattering(false), single_scattering(SingleScattering()) {}

    BSSRDF::BSSRDF(Mesh::ScatteringMaterial & m): material(m), has_single_scattering(false), single_scattering(SingleScattering(m)) {}
    BSSRDF::BSSRDF(Mesh::ScatteringMaterial & m, SingleScattering & s): material(m), has_single_scattering(true), single_scattering(s) {}

    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & wo, const CGLA::Vec3f & no)  {return CGLA::Vec3f(0.0f);}

    Mesh::ScatteringMaterial material;
    bool has_single_scattering;
    SingleScattering single_scattering;

};

#endif // BSSRDF_H
