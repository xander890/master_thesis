#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <string>
#include <vector>
#include <CGLA/Vec4f.h>
#include "Texmap.h"
#include <GLGraphics/ShaderProgram.h>

namespace Mesh
{

    /// A simple struct containing material properties.
    class Material
	{
    public:
        static const std::string AMBIENT_KEY;
        static const std::string DIFFUSE_KEY;
        static const std::string SPECULAR_KEY;
        static const std::string SHININESS_KEY;
        static const std::string TEXTURE_KEY;

        std::string name;
        CGLA::Vec4f ambient;
        CGLA::Vec4f diffuse;
		CGLA::Vec4f specular;

        float shininess;
        Texmap tex_map;
        bool has_texture;

        virtual void loadUniforms(GLGraphics::ShaderProgramDraw & shader);

        Material()
            :name("default"),
              diffuse(0.8f,0.8f,0.8f,1.0f),
              specular(0.0f,0.0f,0.0f,1.f),
              shininess(0),
              has_texture(false),
              ambient(0.0f,0.0f,0.0f,1.0f)
		{
        }
	};
}

#endif
