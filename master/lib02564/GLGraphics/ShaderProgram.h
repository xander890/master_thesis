/*
 *  shaders.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__

#include <string>
#include <GLGraphics/GLHeader.h>
#include <CGLA/Mat4x4f.h>
#include <CGLA/Mat3x3f.h>
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>
#include <CGLA/Mat2x2f.h>

namespace GLGraphics {
    

    /** A generic class for loading and enabling shader programs. Construct with the file names of
     shaders. */
    class ShaderProgram
    {
        std::string shader_path;  // as the name says
        std::string vs_file_name; // Vertex shader file name
        std::string gs_file_name; // geometry shader file name
        std::string fs_file_name; // Fragment shader file name
        GLuint vs, gs, fs; // The shaders

        static int currentShader;
    protected:
        GLuint prog; // The program

        void compile();

        virtual void link();

        /// Destroy shaders and program
        void relinquish();
        
    public:
        
        GLuint id() const {return prog;}

        ShaderProgram()
            : vs(0), gs(0), fs(0), prog(0){

        }

        /// Construct from file names. Note that geometry shader can be omitted by giving an empty string.
        ShaderProgram(const std::string& _shader_path,
                      const std::string& _vs_file_name,
                      const std::string& _gs_file_name,
                      const std::string& _fs_file_name):
        shader_path(_shader_path),
        vs_file_name(_vs_file_name), 
        gs_file_name(_gs_file_name), 
        fs_file_name(_fs_file_name), vs(0), gs(0), fs(0), prog(0)
        {
            reload();
        }
        
        void init(const std::string& _shader_path,
                  const std::string& _vs_file_name,
                  const std::string& _gs_file_name,
                  const std::string& _fs_file_name){
            shader_path = _shader_path;
            vs_file_name = _vs_file_name;
            gs_file_name = _gs_file_name;
            fs_file_name = _fs_file_name;
            reload();
        }

        /// Destroy and relinquish.
       virtual ~ShaderProgram()
        {
            relinquish();
        }


        
        /// Reload shaders that make up program. Nifty if changed on disk.
        virtual void reload();
        
        /// Enable shader. No disable since we might not want to switch back to previous state.
        void use();
        
        /** Returns uniform location. Much nicer for me than maintaining many functions that help _set_ 
         uniforms. Minimal extra overhead for user. */
        GLint get_uniform_location(const std::string& name);

        void set_uniform(const char* name, int value);
        void set_uniform(const char* name, float value);
        void set_uniform(const char* name, const CGLA::Vec2f &value);
        void set_uniform(const char* name, const CGLA::Vec3f &value);
        void set_uniform(const char* name, const CGLA::Vec4f &value);
        void set_uniform(const char* name, const CGLA::Mat4x4f &value);
        void set_uniform(const char* name, const CGLA::Mat3x3f &value);
        void set_uniform(const char* name, const CGLA::Mat2x2f &value);

        void set_uniform(const char* name, std::vector<int> value, unsigned int size);
        void set_uniform(const char* name, std::vector<float> value, unsigned int size);
        void set_uniform(const char* name, const std::vector<CGLA::Vec2f> &value , unsigned int size);
        void set_uniform(const char* name, const std::vector<CGLA::Vec3f> &value, unsigned int size);
        void set_uniform(const char* name, const std::vector<CGLA::Vec4f> &value, unsigned int size);
        void set_uniform(const char* name, const std::vector<CGLA::Mat4x4f> &value, unsigned int size);
        void set_uniform(const char* name, const std::vector<CGLA::Mat3x3f> &value, unsigned int size);

        GLint get_attrib_location(const std::string& name);
        
        void use_texture(GLenum target, const std::string& name, GLuint tex, GLuint active_texture=0);
        
        /**
         * @brief load_a_shader load and compiles a GLSL shader
         * @param shader_type GL_VERTEX_SHADER, GL_GEOMETRY_SHADER or GL_FRAGMENT_SHADER
         * @param shader_path path of shaders (must end with '/')
         * @param str shader file name (or if string starts with '$' then str is used as shader source)
         * @return shader handle or 0 if not created
         */
        static GLuint load_a_shader(GLenum shader_type, const std::string& shader_path, const std::string& str);

        /** Create a shader of type specified by the first argument from a source string given
            as second argument.	Return shader handle. If there is a problem, the infolog is
            printed and 0 is returned. */
        static GLuint create_glsl_shader(GLuint stype, const std::string& src);
    };
    
    
    class ShaderProgramDraw: public ShaderProgram
    {        
        CGLA::Vec4f light_pos;
        CGLA::Vec4f light_diff;
        CGLA::Vec4f light_spec;
        CGLA::Vec4f light_amb;
        
        CGLA::Vec4f mat_diff;
        CGLA::Vec4f mat_spec;
        float mat_spec_exp;
        
        CGLA::Mat4x4f M;
        CGLA::Mat4x4f V;
        CGLA::Mat4x4f P;
        
        void set_derived_matrices();
    protected:
        void link();
    public:
        ShaderProgramDraw(){

        }
        
        /// Construct from file names. Note that geometry shader can be omitted by giving an empty string.
        ShaderProgramDraw(const std::string& _shader_path,
                      const std::string& _vs_file_name,
                      const std::string& _gs_file_name,
                      const std::string& _fs_file_name):
        ShaderProgram(_shader_path, _vs_file_name, _gs_file_name, _fs_file_name),
        light_pos(CGLA::Vec4f(0,0,1,0)),
        M(CGLA::identity_Mat4x4f()),
        V(CGLA::identity_Mat4x4f()),
        P(CGLA::identity_Mat4x4f())
        {
        }

        /// Destroy and relinquish.
        ~ShaderProgramDraw() {}

        void reload();
                
        //void set_light_position(const CGLA::Vec4f& light_pos);
        //void set_light_intensities(const CGLA::Vec4f& diff, const CGLA::Vec4f& spec, const CGLA::Vec4f& amb);
        
        void set_material(const CGLA::Vec4f& diff, const CGLA::Vec4f& spec, float exp);
        
        void set_model_matrix(const CGLA::Mat4x4f& model_matrix);
        
        void set_view_matrix(const CGLA::Mat4x4f& view_matrix);
        
        void set_projection_matrix(const CGLA::Mat4x4f& projection_matrix);
        
        CGLA::Mat4x4f get_view_matrix() const {return V;}
        
        CGLA::Mat4x4f get_model_matrix() const {return M;}
        
        CGLA::Mat4x4f get_projection_matrix() const {return P;}
        
        static GLint get_generic_attrib_location(const std::string& name);
    };
}


#endif
