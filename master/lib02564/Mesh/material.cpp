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

        unsigned int count = 0;
        for(vector<Texture>::iterator it = textures.begin(); it != textures.end(); it++)
        {
            Texture tex_map = *it;
            shader.use_texture(GL_TEXTURE_2D, tex_map.get_name(), tex_map.get_id(), count);
            count++;
        }
        loadExtraUniforms(shader);
    }

    void Material::addTexture(Texture & texture)
    {
        textures.push_back(texture);
    }

    void Material::initTextures()
    {
        for(std::vector<Texture>::iterator it = textures.begin(); it != textures.end(); it++)
        {
            it->init();
        }
    }


    //uninteresting stuff under this line -.-
    void Material::addUniform(const char *name, int value)
    {
        intUniforms[name] = value;
    }

    void Material::addUniform(const char *name, float value)
    {
        floatUniforms[name] = value;
    }

    void Material::addUniform(const char *name, Vec2f value)
    {
        vec2Uniforms[name] = value;
    }

    void Material::addUniform(const char *name, Vec3f value)
    {
        vec3Uniforms[name] = value;
    }

    void Material::addUniform(const char *name, Vec4f value)
    {
        vec4Uniforms[name] = value;
    }


    void Material::loadExtraUniforms(GLGraphics::ShaderProgramDraw &shader)
    {
        for(map<string,int>::iterator it = intUniforms.begin(); it != intUniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,float>::iterator it = floatUniforms.begin(); it != floatUniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,Vec2f>::iterator it = vec2Uniforms.begin(); it != vec2Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,Vec3f>::iterator it = vec3Uniforms.begin(); it != vec3Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,Vec4f>::iterator it = vec4Uniforms.begin(); it != vec4Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
    }
}
