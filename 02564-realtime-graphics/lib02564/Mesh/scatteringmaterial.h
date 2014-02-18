#ifndef SCATTERINGMATERIAL_H
#define SCATTERINGMATERIAL_H

#include "Material.h"

namespace Mesh{

class ScatteringMaterial : public Mesh::Material
{
public:
    ScatteringMaterial();
    ScatteringMaterial(Mesh::Material m, float indexOfRefraction, CGLA::Vec3f absorption, CGLA::Vec3f scattering, CGLA:: Vec3f meancosine);

    // base parameters
    const float indexOfRefraction;
    const CGLA::Vec3f absorption;
    const CGLA::Vec3f scattering;
    const CGLA::Vec3f meancosine;

    // derived parameters
    const CGLA::Vec3f D;
    const CGLA::Vec3f reducedExtinctionCoefficent;
    const CGLA::Vec3f transmissionCoefficient;
    const CGLA::Vec3f reducedScatteringCoefficient;

private:
    computeCoefficients();
};

}
#endif // SCATTERINGMATERIAL_H
