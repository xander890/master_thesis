#ifndef __OBJ_LOADER_H
#define __OBJ_LOADER_H

#include <vector>
#include <GLGraphics/GLHeader.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <Mesh/Material.h>

namespace Mesh {

class ObjLoader
{
public:
// Load an OBJ model into the out parameters.
// Note only simple OBJ files are supported.
static bool load_object(const char * filename,
    std::vector<CGLA::Vec3f> *out_positions,
    std::vector<std::vector<GLuint> > *out_indices,
    std::vector<Mesh::Material> *out_materials,
    std::vector<CGLA::Vec3f> *out_normal = NULL,
    std::vector<CGLA::Vec2f> *out_uv = NULL,
    float scale = 1.0f
	);

};
}

#endif
