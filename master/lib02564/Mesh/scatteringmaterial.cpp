#include "scatteringmaterial.h"
#include "Utils/miscellaneous.h"

using namespace CGLA;

namespace Mesh{

    const std::string ScatteringMaterial::IOR = "ior";
    const std::string ScatteringMaterial::ABSORPTION = "absorption";
    const std::string ScatteringMaterial::SCATTERING = "scattering";
    const std::string ScatteringMaterial::MEAN_COSINE = "mean_cosine";
    const std::string ScatteringMaterial::D_COEFF = "D";
    const std::string ScatteringMaterial::EXTINCTION = "extinction";
    const std::string ScatteringMaterial::REDUCED_EXTINCTION = "red_extinction";
    const std::string ScatteringMaterial::REDUCED_SCATTERING = "red_scattering";
    const std::string ScatteringMaterial::TRANSMISSION = "transmission";
    const std::string ScatteringMaterial::FRESNEL_INTEGRAL_SIGMA = "C_s";
    const std::string ScatteringMaterial::FRESNEL_INTEGRAL_E = "C_e";
    const std::string ScatteringMaterial::FRESNEL_INTEGRAL_SIGMA_INV = "C_s_inv";
    const std::string ScatteringMaterial::A_COEFF = "A";
    const std::string ScatteringMaterial::DE_COEFF = "de";
    const std::string ScatteringMaterial::REDUCED_ALBEDO = "reduced_albedo";
    const std::string ScatteringMaterial::GLOBAL_COEFF= "global_coeff";
    const std::string ScatteringMaterial::THREE_D= "three_D";
    const std::string ScatteringMaterial::D_REV= "D_rev";
    const std::string ScatteringMaterial::TWO_A_DE= "two_a_de";


    void ScatteringMaterial::loadUniforms(GLGraphics::ShaderProgramDraw &shader)
    {
        Material::loadUniforms(shader);
        shader.set_uniform(IOR.c_str(),indexOfRefraction);
        shader.set_uniform(ABSORPTION.c_str(),absorption);
        shader.set_uniform(SCATTERING.c_str(),scattering);
        shader.set_uniform(MEAN_COSINE.c_str(),meancosine);
        shader.set_uniform(D_COEFF.c_str(),D);
        shader.set_uniform(EXTINCTION.c_str(),extinctionCoefficient);
        shader.set_uniform(REDUCED_EXTINCTION.c_str(),reducedExtinctionCoefficent);
        shader.set_uniform(REDUCED_SCATTERING.c_str(),reducedScatteringCoefficient);
        shader.set_uniform(TRANSMISSION.c_str(),transmissionCoefficient);
        shader.set_uniform(FRESNEL_INTEGRAL_SIGMA.c_str(),C_s);
        shader.set_uniform(FRESNEL_INTEGRAL_E.c_str(),C_E);
        shader.set_uniform(FRESNEL_INTEGRAL_SIGMA_INV.c_str(),C_s_inv);
        shader.set_uniform(A_COEFF.c_str(),A);
        shader.set_uniform(DE_COEFF.c_str(),de);
        shader.set_uniform(REDUCED_ALBEDO.c_str(),reducedAlbedo);
        shader.set_uniform(THREE_D.c_str(), three_D);
        shader.set_uniform(D_REV.c_str(), rev_D);
        shader.set_uniform(GLOBAL_COEFF.c_str(), global_coeff);
        shader.set_uniform(TWO_A_DE.c_str(), two_a_de);
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
        this->C_s = C_Sigma(indexOfRefraction);
        this->C_s_inv = C_Sigma(1.0f/indexOfRefraction);
        this->C_E = C_e(indexOfRefraction);
        this->reducedAlbedo = reducedScatteringCoefficient / reducedExtinctionCoefficent;
        this->de = 2.131 * D / Vec3f(
                    (float)sqrt(reducedAlbedo[0]),
                    (float)sqrt(reducedAlbedo[1]),
                    (float)sqrt(reducedAlbedo[2]));
        this->A = (1.0 - C_E) / (2.0 * C_s);
        this->extinctionCoefficient = scattering + absorption;
        this->three_D = 3 * D;
        this->rev_D = (3.f * reducedExtinctionCoefficent);
        this->two_a_de = 2.0f * A * de;
        this->global_coeff = 1.0f/(4.0f * C_s_inv) * 1.0f/(4.0f * M_PI * M_PI);
    }
}
