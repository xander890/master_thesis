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
		std::string name;			   
        CGLA::Vec4f diffuse;
		CGLA::Vec4f specular;
        float shininess;
        Texmap tex_map;
        bool has_texture;
        CGLA::Vec4f ambient;
        CGLA::Vec4f transmission;
        float illum;
        float ior;



        Material()
            :name("default"),
              diffuse(0.8f,0.8f,0.8f,1.0f),
              specular(0.0f,0.0f,0.0f,1.f),
              shininess(0),
              has_texture(false),
              ambient(0.0f,0.0f,0.0f,1.0f),
              transmission(0.0f,0.0f,0.0f,1.0f),
              illum(0),
              ior(0)
		{
        }
	};
}

#endif
