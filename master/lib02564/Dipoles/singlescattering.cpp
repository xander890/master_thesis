#include "singlescattering.h"
#include <vector>
#include "Utils/miscellaneous.h"
#include "Utils/cglautils.h"

using namespace std;
using namespace CGLA;

//single scattering without multiplication Li (added in RTE evaluation) and F_t_out (added in shader).
CGLA::Vec3f SingleScattering::evaluate(const CGLA::Vec3f &xi, const CGLA::Vec3f &wi, const CGLA::Vec3f &ni, const CGLA::Vec3f &xo, const CGLA::Vec3f &wo, const CGLA::Vec3f &no)
{
    float F = fresnel_T(wi,ni,1.0f,material.indexOfRefraction);
    Vec3f wi_refr = refract(wi,ni,1.0f,material.indexOfRefraction);
    Vec3f wo_refr = refract(wo,no,1.0f,material.indexOfRefraction);

    float G = abs(dot(wo_refr,ni)/dot(wi_refr,ni));
    Vec3f sigma_tc = (G + 1) * material.extinctionCoefficient;
    float phase_function = 1.0f / (4.0f * M_PI);

    float dot_ni_wi = dot(wi,ni);
    float refraction_conversion = abs(dot_ni_wi)/sqrt(1 - pow((1.0/material.indexOfRefraction),2) * ( 1.0f - dot_ni_wi * dot_ni_wi));
    float si_prime = length(xo - xi) * refraction_conversion;
    Vec3f exp_1 = -material.extinctionCoefficient * si_prime;

    float zeta = 0.5;
    float outgoing_term = 1.0f/zeta; // exp(-log(zeta)) = 1 / zeta
    Vec3f incoming_term = expVec3f(exp_1); // exp(-si_prime * sigma_t)

    Vec3f coefficients = (material.scattering * phase_function * F) / sigma_tc;
    return coefficients * incoming_term * outgoing_term;
}
