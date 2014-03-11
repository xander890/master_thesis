#ifndef SCATTERINGMATERIAL_H
#define SCATTERINGMATERIAL_H

#include "Material.h"

namespace Mesh{

class ScatteringMaterial : public Mesh::Material
{
public:

    ScatteringMaterial(float indexOfRefraction, CGLA::Vec3f absorption, CGLA::Vec3f scattering, CGLA:: Vec3f meancosine) :
        indexOfRefraction(indexOfRefraction),
        scattering(scattering),
        absorption(absorption),
        meancosine(meancosine)
    {
         computeCoefficients();
    }

    virtual void loadUniforms(GLGraphics::ShaderProgramDraw & shader) override;

    // base parameters
     float indexOfRefraction; //The usual assumption is that this can be intercheangeably the material ior or the ratio between it and air (ior = 1)
     CGLA::Vec3f absorption;
     CGLA::Vec3f scattering;
     CGLA::Vec3f meancosine;

    // derived parameters
     CGLA::Vec3f D;
     CGLA::Vec3f reducedExtinctionCoefficent;
     CGLA::Vec3f transmissionCoefficient;
     CGLA::Vec3f reducedScatteringCoefficient;
     float C_s;
     float C_s_inv;
     float C_E;
     float A;
     CGLA::Vec3f de;
     CGLA::Vec3f reducedAlbedo;
     CGLA::Vec3f extinctionCoefficient;

     static const std::string IOR;
     static const std::string ABSORPTION;
     static const std::string SCATTERING;
     static const std::string MEAN_COSINE;
     static const std::string D_COEFF;
     static const std::string EXTINCTION;
     static const std::string REDUCED_EXTINCTION;
     static const std::string REDUCED_SCATTERING;
     static const std::string TRANSMISSION;
     static const std::string FRESNEL_INTEGRAL_SIGMA;
     static const std::string FRESNEL_INTEGRAL_E;
     static const std::string FRESNEL_INTEGRAL_SIGMA_INV;
     static const std::string A_COEFF;
     static const std::string DE_COEFF;
     static const std::string REDUCED_ALBEDO;



private:
    void computeCoefficients();
};

}
#endif // SCATTERINGMATERIAL_H
