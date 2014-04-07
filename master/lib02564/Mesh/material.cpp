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
            Texture texture = *it;
            shader.use_texture(texture.get_target(), texture.get_name(), texture.get_id(), count);
            count++;
        }
        loadExtraUniforms(shader);

        //uniforms
        if(receives_shadows)
        {
            GLint shadowloc = shader.get_uniform_location(SHADOW_TEX_ID);
            if(shadowloc != -1)
            {
                shadowbuffer->bind_textures(shadowloc);
                shader.set_uniform(SHADOW_TEX_ID, shadowloc);
                shader.set_uniform(SHADOW_ENABLED, 1);
                shader.set_uniform(SHADOW_MATRIX_ID, shadowbuffer->getLightWorldTransformationMatrix());
            }
        }
        else
        {
            shader.set_uniform(SHADOW_ENABLED, 0);
        }

    }

    void Material::addTexture(Texture & texture)
    {
        for(int i = 0; i < textures.size(); i++)
        {
            if(texture.get_name().compare(textures[i].get_name()) == 0)
            {
                return;
            }
        }
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

    void Material::addUniform(const char *name, Mat2x2f value)
    {
        mat2Uniforms[name] = value;
    }

    void Material::addUniform(const char *name, Mat3x3f value)
    {
        mat3Uniforms[name] = value;
    }

    void Material::addUniform(const char *name, Mat4x4f value)
    {
        mat4Uniforms[name] = value;
    }

    void Material::setShadowBuffer(ShadowBuffer * buffer)
    {
        shadowbuffer = buffer;
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
        for(map<string,Mat2x2f>::iterator it = mat2Uniforms.begin(); it != mat2Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,Mat3x3f>::iterator it = mat3Uniforms.begin(); it != mat3Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
        for(map<string,Mat4x4f>::iterator it = mat4Uniforms.begin(); it != mat4Uniforms.end(); it++)
        {
            shader.set_uniform(it->first.c_str(),it->second);
        }
    }
}
