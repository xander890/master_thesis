#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H
#include <vector>
#include <map>
#include <GLGraphics/GLHeader.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <CGLA/Mat3x3f.h>
#include <CGLA/Mat4x4f.h>

void duplicateMap(const std::vector<CGLA::Vec3f> & originalVector, std::map<GLuint, std::vector<GLuint>> & duplicates, float tolerance);


CGLA::Vec3f refract(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2);

// These functions return the fresnel terms based on polarizations.
CGLA::Vec2f fresnelAmplitudeReflectance(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2);
CGLA::Vec2f fresnelAmplitudeTransmittance(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2);
CGLA::Vec2f fresnelPowerReflectance(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2);
CGLA::Vec2f fresnelPowerTransmittance(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2);

inline float fresnel_R(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2)
{
    CGLA::Vec2f R = fresnelPowerReflectance(in,n,n1,n2);
    float RR = 0.5f * (R[0] + R[1]);
    return RR;
}

inline float fresnel_T(const CGLA::Vec3f & in, const CGLA::Vec3f & n, float n1, float n2)
{
    CGLA::Vec2f T = fresnelPowerTransmittance(in,n,n1,n2);
    float TT = 0.5f * (T[0] + T[1]);
    return TT;
}

void debugFresnelTerms();
float C_Sigma(float ni);
float C_e(float ni);
float C_1(float ni);
float C_2(float ni);

#endif // MISCELLANEOUS_H
