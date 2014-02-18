#include "scatteringmaterial.h"
using namespace CGLA;

namespace Mesh{
    ScatteringMaterial::ScatteringMaterial() : Material()
    {
        this->indexOfRefraction = 1.0f;
        this->absorption = Vec3f(0.01f);
        this->scattering = Vec3f(1.0f);
        this->meancosine = Vec3f(0.0f);
        computeCoefficients();
    }

    ScatteringMaterial::ScatteringMaterial(Material m, float indexOfRefraction, CGLA::Vec3f absorption, CGLA::Vec3f scattering, CGLA::Vec3f meancosine)
    {
        this->ambient = m.ambient;
        this->diffuse = m.diffuse;
        this->has_texture = m.has_texture;
        this->illum = m.illum;
        this->ior = m.ior;
        this->name = m.name;
        this->shininess = m.shininess;
        this->specular = m.specular;
        this->transmission = m.transmission;
        this->tex_map = m.tex_map;

        this->indexOfRefraction = indexOfRefraction;
        this->absorption = absorption;
        this->scattering = scattering;
        this->meancosine = meancosine;
        computeCoefficients();
    }

    ScatteringMaterial::computeCoefficients()
    {
        this->reducedScatteringCoefficient = scattering * (1 - meancosine);
        this->reducedExtinctionCoefficent = reducedScatteringCoefficient * absorption;
        this->D = Vec3f(1.0f) / (3.f * reducedExtinctionCoefficent);
        this->transmissionCoefficient = Vec3f(
                    (float)sqrt(3*absorption[0]*reducedExtinctionCoefficent[0]),
                    (float)sqrt(3*absorption[1]*reducedExtinctionCoefficent[1]),
                    (float)sqrt(3*absorption[2]*reducedExtinctionCoefficent[2]));
    }


}
