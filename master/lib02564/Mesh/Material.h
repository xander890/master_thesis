#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <string>
#include <vector>
#include <CGLA/Vec4f.h>
#include "texture.h"
#include <GLGraphics/ShaderProgram.h>
#include <ShadowBuffer.h>
#include <CGLA/Mat2x2f.h>


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

        bool has_texture;
        bool cast_shadows;
        bool receives_shadows;

        virtual void loadUniforms(GLGraphics::ShaderProgramDraw & shader);

        virtual void addTexture(Texture * texture);
        virtual Texture *getTexture(std::string &name);

        virtual void initTextures();

        virtual void addUniform(const char* name, int value);
        virtual void addUniform(const char* name, float value);
        virtual void addUniform(const char* name, CGLA::Vec2f value);
        virtual void addUniform(const char* name, CGLA::Vec3f value);
        virtual void addUniform(const char* name, CGLA::Vec4f value);
        virtual void addUniform(const char* name, CGLA::Mat2x2f value);
        virtual void addUniform(const char* name, CGLA::Mat3x3f value);
        virtual void addUniform(const char* name, CGLA::Mat4x4f value);

        virtual void setShadowBuffer(ShadowBuffer * buffer);

        Material()
            :name("default"),
              diffuse(0.8f,0.8f,0.8f,1.0f),
              specular(0.0f,0.0f,0.0f,1.f),
              shininess(0),
              has_texture(false),
              ambient(0.0f,0.0f,0.0f,1.0f),
              textures(std::vector<Texture *>()),
              cast_shadows(true),
              receives_shadows(true),
              shadowbuffer(new ShadowBuffer(0))
		{

        }

    private:
        ShadowBuffer * shadowbuffer;
        void loadExtraUniforms(GLGraphics::ShaderProgramDraw &shader);
        std::vector<Texture*> textures;
        std::map<std::string, int> intUniforms;
        std::map<std::string, float> floatUniforms;
        std::map<std::string, CGLA::Vec2f> vec2Uniforms;
        std::map<std::string, CGLA::Vec3f> vec3Uniforms;
        std::map<std::string, CGLA::Vec4f> vec4Uniforms;
        std::map<std::string, CGLA::Mat2x2f> mat2Uniforms;
        std::map<std::string, CGLA::Mat3x3f> mat3Uniforms;
        std::map<std::string, CGLA::Mat4x4f> mat4Uniforms;

	};
}

#endif
