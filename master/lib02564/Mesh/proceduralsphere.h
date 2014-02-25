#ifndef PROCEDURALSPHERE_H
#define PROCEDURALSPHERE_H
#include <vector>
#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <GLGraphics/GLHeader.h>

namespace GLGraphics
{
   void sphere(float r, int w0, int w1, std::vector<CGLA::Vec3f>& vertex, std::vector<CGLA::Vec3f>& normal, std::vector<CGLA::Vec2f>& texcoord);
   GLuint create_sphere_vertex_array_object(std::vector<CGLA::Vec3f>& vertex, std::vector<CGLA::Vec3f>& normal, std::vector<CGLA::Vec2f>& texcoord);

}
#endif // PROCEDURALSPHERE_H
