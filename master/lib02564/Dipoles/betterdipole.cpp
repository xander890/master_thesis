#include "betterdipole.h"

#include "Utils/cglautils.h"
#include "Utils/miscellaneous.h"

using namespace std;
using namespace CGLA;

CGLA::Vec3f BetterDipole::evaluate(const CGLA::Vec3f &xi, const CGLA::Vec3f &wi, const CGLA::Vec3f &ni, const CGLA::Vec3f &xo, const CGLA::Vec3f &wo, const CGLA::Vec3f &no)
{
    float l = length(xo - xi);

    float ntr = material.indexOfRefraction;
    float nin = 1.0f; //air
    float eta = ntr / nin;


    Vec3f D = (2 * material.absorption + material.reducedScatteringCoefficient) / (3 * material.reducedExtinctionCoefficent * material.reducedExtinctionCoefficent);
    float A = (1 + 3 * C_2(eta)) / (1 - 2 * C_1(eta));

    Vec3f zr = invertVec3f(material.reducedExtinctionCoefficent);
    Vec3f zv = - zr - 4.0f * A * D;

    Vec3f dr;
    Vec3f dv;

    Vec3f r = Vec3f(l);
    dr = sqrtVec3f(r * r + zr * zr);
    dv = sqrtVec3f(r * r + zv * zv);

    Vec3f tr = sqrtVec3f(material.absorption / D);

    Vec3f C1 = (zr / (dr * dr)) * (tr * dr + Vec3f(1.0f));
    Vec3f C2 = (zv / (dv * dv)) * (tr * dv + Vec3f(1.0f));

    float CE = C_e(eta);
    Vec3f reducedCSigma = Vec3f(C_Sigma(eta)) / D;
    Vec3f coeff = (material.reducedAlbedo * material.reducedAlbedo) / (4.0f * M_PI);
    Vec3f real = (CE * C1 + reducedCSigma) * (expVec3f(- tr * dr)/dr);
    Vec3f virt = (CE * C2 + reducedCSigma) * (expVec3f(- tr * dv)/dv);

    Vec3f S = coeff * (real - virt);
    float Ti = fresnel_T(wi,ni,nin,ntr);
//    float To = fresnel_T(wo,no,nin,ntr);

    S *= (1.0f/M_PI)* Ti;// * To;

    for(int i = 0; i < 3 ; i++)
    {
        S[i] = std::max(0.0f,S[i]);
    }

    return S;
}
