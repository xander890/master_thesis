#ifndef SINGLESCATTERING_H
#define SINGLESCATTERING_H

#include "Mesh/scatteringmaterial.h"
#include <vector>
#include <CGLA/Vec3f.h>

class SingleScattering
{
public:
    SingleScattering::SingleScattering() : material(Mesh::ScatteringMaterial()) {}
    SingleScattering::SingleScattering(Mesh::ScatteringMaterial & mat) : material(mat) {}

    virtual CGLA::Vec3f evaluate(const CGLA::Vec3f & xi, const CGLA::Vec3f & wi, const CGLA::Vec3f & ni,
                                 const CGLA::Vec3f & xo, const CGLA::Vec3f & wo, const CGLA::Vec3f & no);
private:
    Mesh::ScatteringMaterial material;
};

#endif // SINGLESCATTERING_H
