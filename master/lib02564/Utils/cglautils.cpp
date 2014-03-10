#include "cglautils.h"

using namespace CGLA;

void convertMat4x4toMat3x3(const CGLA::Mat4x4f &in, CGLA::Mat3x3f &out)
{
    out[0][0] = in[0][0];
    out[0][1] = in[0][1];
    out[0][2] = in[0][2];
    out[1][0] = in[1][0];
    out[1][1] = in[1][1];
    out[1][2] = in[1][2];
    out[2][0] = in[2][0];
    out[2][1] = in[2][1];
    out[2][2] = in[2][2];
}


Vec3f invertVec3f(const Vec3f vector)
{
    return Vec3f(1.0f) / vector;
}

Vec3f sqrtVec3f(const Vec3f vector)
{
    return Vec3f(sqrt(vector[0]),sqrt(vector[1]),sqrt(vector[2]));
}

Vec3f powVec3f(const Vec3f vector, const float exp)
{
    return Vec3f(pow(vector[0],exp),pow(vector[1],exp),pow(vector[2],exp));
}


Vec3f expVec3f(const Vec3f vector)
{
    return Vec3f(expf(vector[0]),expf(vector[1]),expf(vector[2]));
}
