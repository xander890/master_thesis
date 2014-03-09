#ifndef CGLAUTILS_H
#define CGLAUTILS_H

#include <GLGraphics/GLHeader.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Mat3x3f.h>
#include <CGLA/Mat4x4f.h>

void convertMat4x4toMat3x3(const CGLA::Mat4x4f & in, CGLA::Mat3x3f & out);
CGLA::Vec3f invertVec3f(const CGLA::Vec3f vector);
CGLA::Vec3f sqrtVec3f(const CGLA::Vec3f vector);
CGLA::Vec3f powVec3f(const CGLA::Vec3f vector, const float exp);


#endif // CGLAUTILS_H
