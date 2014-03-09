#ifndef MISCELLANEOUS_H
#define MISCELLANEOUS_H
#include <vector>
#include <map>
#include <GLGraphics/GLHeader.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Mat3x3f.h>
#include <CGLA/Mat4x4f.h>

void duplicateMap(const std::vector<CGLA::Vec3f> & originalVector, std::map<GLuint, std::vector<GLuint>> & duplicates, float tolerance);

#endif // MISCELLANEOUS_H
