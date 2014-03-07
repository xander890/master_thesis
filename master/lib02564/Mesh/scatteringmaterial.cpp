#include "scatteringmaterial.h"
using namespace CGLA;

namespace Mesh{
    ScatteringMaterial::ScatteringMaterial() : Material()
    {
        this->indexOfRefraction = 1.3f;
        this->absorption = Vec3f(0.1f);
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

    void ScatteringMaterial::computeCoefficients()
    {
        this->reducedScatteringCoefficient = scattering * (Vec3f(1.0f) - meancosine);
        this->reducedExtinctionCoefficent = reducedScatteringCoefficient + absorption;
        this->D = Vec3f(1.0f) / (3.f * reducedExtinctionCoefficent);
        this->transmissionCoefficient = Vec3f(
                    (float)sqrt(3*absorption[0]*reducedExtinctionCoefficent[0]),
                    (float)sqrt(3*absorption[1]*reducedExtinctionCoefficent[1]),
                    (float)sqrt(3*absorption[2]*reducedExtinctionCoefficent[2]));
        this->T12 = 4.0f * pow((1.0f/indexOfRefraction),2) / pow(1.0f + 1.0f/indexOfRefraction,2);
        this->T21 = 4.0f * pow((indexOfRefraction),2) / pow(1.0f + indexOfRefraction,2);
        this->C_s = C_Sigma(indexOfRefraction);
        this->C_s_inv = C_Sigma(1.0f/indexOfRefraction);
        this->C_E = C_e(indexOfRefraction);
        this->reducedAlbedo = reducedScatteringCoefficient / reducedExtinctionCoefficent;
        this->de = 2.131 * D / Vec3f(
                    (float)sqrt(reducedAlbedo[0]),
                    (float)sqrt(reducedAlbedo[1]),
                    (float)sqrt(reducedAlbedo[2]));
        this->A = (1.0 - C_E) / (2.0 * C_s);
    }

    float ScatteringMaterial::C_Sigma(float ni)
    {
        float c;
        if(ni < 1.0f)
        {
            c = + 0.919317
                - 3.4793  * ni
                + 6.75335 * pow(ni,2)
                - 7.80989 * pow(ni,3)
                + 4.98554 * pow(ni,4)
                - 1.36881 * pow(ni,5);
        }
        else
        {
            c = - 9.23372
                + 22.2272  * ni
                - 20.9292  * pow(ni,2)
                + 10.2291  * pow(ni,3)
                - 2.54396  * pow(ni,4)
                + 0.254913 * pow(ni,5);
        }
        return 0.25 * (1 - c);
    }



    float ScatteringMaterial::C_e(float ni)
    {
        float c;
        if(ni < 1.0f)
        {
            c = + 0.828421
                - 2.62051  * ni
                + 3.36231  * pow(ni , 2)
                - 1.95284  * pow(ni , 3)
                + 0.236494 * pow(ni , 4)
                + 0.145787 * pow(ni,5);
        }
        else
        {
            c = - 1641.1
                + 135.926 / pow(ni,3)
                - 656.175 / pow(ni,2)
                + 1376.53 / ni
                + 1213.67 * ni
                - 568.556 * pow(ni,2)
                + 164.798 * pow(ni,3)
                - 27.0181 * pow(ni,4)
                + 1.91826 * pow(ni,5);
        }
        return 0.5 * (1 - c);
    }

}
