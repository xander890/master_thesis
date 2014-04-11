#ifndef AREAESTIMATOR_H
#define AREAESTIMATOR_H
#include "Mesh/TriangleMesh.h"
#include "CGLA/Vec3f.h"
#include "CGLA/Mat3x3f.h"
#include "GLGraphics/ThreeDObject.h"

void totalArea(GLGraphics::ThreeDObject & obj, float & result);
void totalArea(Mesh::TriangleMesh & mesh, GLenum mode, CGLA::Mat3x3f modelMatrix, float & result);

void perVertexArea(Mesh::TriangleMesh & mesh, GLenum mode, CGLA::Mat3x3f modelMatrix, std::vector<float> & result);

float area(CGLA::Vec3f & v1, CGLA::Vec3f & v2, CGLA::Vec3f & v3);

#endif // AREAESTIMATOR_H
