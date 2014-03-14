#include "material.h"

using namespace std;
using namespace CGLA;

namespace Mesh
{
    const std::string Material::AMBIENT_KEY = "mat_amb";
    const std::string Material::DIFFUSE_KEY = "mat_diff";
    const std::string Material::SPECULAR_KEY = "mat_spec";
    const std::string Material::SHININESS_KEY = "mat_spec_exp";
    const std::string Material::TEXTURE_KEY = "tex";

    void Material::loadUniforms(GLGraphics::ShaderProgramDraw &shader)
    {
        shader.set_uniform(AMBIENT_KEY.c_str(),ambient);
        shader.set_uniform(DIFFUSE_KEY.c_str(), diffuse);
        shader.set_uniform(SPECULAR_KEY.c_str(), specular);
        shader.set_uniform(SHININESS_KEY.c_str(), shininess);
        //shader.use_texture(GL_TEXTURE_2D, TEXTURE_KEY.c_str(), tex_map.get_id(), 0);
    }
}
