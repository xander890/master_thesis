/*
 *  shaders.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 31/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <QtGlobal>
#include <QDebug>
#include <iostream>
#include <CGLA/Mat3x3f.h>

#include "ShaderProgram.h"
#include "ResourceLoader.h"

namespace GLGraphics {
    using namespace std;
    using namespace CGLA;
    
    int ShaderProgram::currentShader = -1;
    ShaderPreprocessor ShaderProgram::preprocessor = ShaderPreprocessor("./shaders/TranslucentMaterials/");
    
    void ShaderProgram::relinquish()
    {
        glDeleteProgram(prog);
        glDeleteShader(vs);
        glDeleteShader(gs);
        glDeleteShader(fs);
    }
    
    void ShaderProgram::compile()
    {
        prog = glCreateProgram();

        vs = load_a_shader(GL_VERTEX_SHADER, shader_path,vs_file_name);

        if(gs_file_name != "")
            gs = load_a_shader(GL_GEOMETRY_SHADER, shader_path, gs_file_name);

        fs = load_a_shader(GL_FRAGMENT_SHADER, shader_path, fs_file_name );

        if(vs) glAttachShader(prog, vs);
        if(gs) glAttachShader(prog, gs);
        if(fs) glAttachShader(prog, fs);
    }

    void ShaderProgram::link()
    {

        glLinkProgram(prog);
        GLint  linked;

        glGetProgramiv( prog, GL_LINK_STATUS, &linked );
        if ( !linked ) {
            std::cerr << "Shader program failed to link" << std::endl;
            GLint  logSize;
            glGetProgramiv( prog, GL_INFO_LOG_LENGTH, &logSize);
            char* logMsg = new char[logSize];
            glGetProgramInfoLog( prog, logSize, NULL, logMsg );
            std::cerr << logMsg << std::endl;
            delete [] logMsg;
        }
    }

    void ShaderProgram::set_uniform(const char* name, int value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform1i(location, value);
        }
    }

    void ShaderProgram::set_uniform(const char* name, float value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform1f(location, value);
        }
    }
    void ShaderProgram::set_uniform(const char* name, const CGLA::Vec2f &value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform2fv(location, 1, value.get());
        }
    }
    void ShaderProgram::set_uniform(const char* name, const CGLA::Vec3f &value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform3fv(location, 1, value.get());
        }
    }

    void ShaderProgram::set_uniform(const char* name, const CGLA::Vec4f &value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform4fv(location, 1, value.get());
        }
    }

    void ShaderProgram::set_uniform(const char* name, const CGLA::Mat3x3f &value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            CGLA::Mat3x3f transposed;
            transpose(value, transposed);
            glUniformMatrix3fv(location, 1, GL_FALSE, transposed.get());
        }
    }

    void ShaderProgram::set_uniform(const char *name, const Mat2x2f &value)
    {
        GLint location = get_uniform_location(name);
        if (location != -1){
            CGLA::Mat2x2f transposed;
            transpose(value, transposed);
            glUniformMatrix2fv(location, 1, GL_FALSE, transposed.get());
        }
    }


    void ShaderProgram::set_uniform(const char* name, const CGLA::Mat4x4f &value){
        GLint location = get_uniform_location(name);
        if (location != -1){
            CGLA::Mat4x4f transposed;
            transpose(value, transposed);
            glUniformMatrix4fv(location, 1, GL_FALSE, transposed.get());
        }
    }


    void ShaderProgram::set_uniform(const char* name, vector<float> value, unsigned int size){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform1fv(location, size, value.data());
        }
    }

    void ShaderProgram::set_uniform(const char* name, const vector<CGLA::Vec2f> &value, unsigned int size){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform2fv(location, size, (const GLfloat *)&value[0]);
        }
    }
    void ShaderProgram::set_uniform(const char* name, const vector<CGLA::Vec3f> &value, unsigned int size){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform3fv(location, size, (const GLfloat *)&value[0]);

        }
    }
    void ShaderProgram::set_uniform(const char* name, const vector<CGLA::Vec4f> &value, unsigned int size){
        GLint location = get_uniform_location(name);
        if (location != -1){
            glUniform4fv(location, size, (const GLfloat *)&value[0]);

        }
    }

    void ShaderProgram::set_uniform(const char* name, const vector<CGLA::Mat3x3f> &value, unsigned int size){
        GLint location = get_uniform_location(name);

        if (location != -1){
            glUniformMatrix3fv(location, size, GL_TRUE, (const GLfloat *)&value[0]);

        }
    }


    void ShaderProgram::set_uniform(const char* name, const vector<CGLA::Mat4x4f> &value, unsigned int size){
        GLint location = get_uniform_location(name);

        if (location != -1){
            glUniformMatrix4fv(location, size, GL_TRUE, (const GLfloat *)&value[0]);
        }
    }

    void ShaderProgram::reload()
    {
        relinquish();
        compile();
        link();
    }
    
    void ShaderProgram::use()
    {
        glUseProgram(prog);
        currentShader = prog;
    }
    
    
    GLint ShaderProgram::get_uniform_location(const std::string& name)
    {
#ifndef NDEBUG
        assert(0 != prog); // program must have been compiled and linked

        // assumes that the shader uniform will be used immediately (therefor the current program must be used)
        assert(currentShader == prog);
#endif
        return glGetUniformLocation(prog, name.c_str());
    }
    
    
    GLint ShaderProgram::get_attrib_location(const std::string& name)
    {
#ifndef NDEBUG
        assert(0 != prog); // program must have been compiled and linked
#endif
        return glGetAttribLocation(prog, name.c_str());
    }
    
    void ShaderProgram::use_texture(Mesh::Texture * tex, GLuint active_texture)
    {
        GLint u = get_uniform_location(tex->get_name());
        glActiveTexture(GL_TEXTURE0 + active_texture);
        tex->bind();
        glUniform1i(u, active_texture);
    }

    GLuint ShaderProgram::load_a_shader(GLenum shader_type, const string& shader_path, const string& str)
    {
        GLuint shaderProgramId;
        if(str[0] == '$')
            shaderProgramId = ShaderProgram::create_glsl_shader(shader_type, str.substr(1));
        else
        {
            string fullPath = shader_path+str;
            string shaderSource = ResourceLoader::load_text_resource(fullPath);
            shaderProgramId = ShaderProgram::create_glsl_shader(shader_type, shaderSource);
        }
        if(shaderProgramId==0)
            cout << "In shader: " << str.c_str() << endl;
        return shaderProgramId;
    }

    GLuint ShaderProgram::create_glsl_shader(GLuint stype, std::string& src)
    {
        GLuint S = glCreateShader(stype);

        preprocessor.processShader(src);

        const char* src_cstr = src.c_str();

        glShaderSource(S, 1, &src_cstr, 0);
        glCompileShader(S);

        GLint status;
        glGetShaderiv(S,GL_COMPILE_STATUS, &status);
        if (status == 0)
        {
            GLint infologLength = 0;
            glGetShaderiv(S, GL_INFO_LOG_LENGTH, &infologLength);
            cout << src << endl;
            if (infologLength > 0)
            {
                char* infoLog = new char[infologLength];
                glGetShaderInfoLog(S, infologLength, 0, infoLog);
                cout << "InfoLog: " << infoLog << endl;
                delete infoLog;
            }

            return 0;
        }
        return S;
    }

    void ShaderProgramDraw::reload()
    {
        relinquish();
        compile();
        glBindAttribLocation(prog, 0, "vertex");
        glBindAttribLocation(prog, 1, "normal");
        glBindAttribLocation(prog, 2, "texcoord");
        link();
    }

    void ShaderProgramDraw::set_derived_matrices()
    {
        Mat4x4f VM = V*M;

        set_uniform("VM", VM);
        Mat4x4f PVM = P*VM;
        set_uniform("PVM", PVM);
        Mat4x4f N4 = invert(transpose(VM));
        Mat3x3f N;
        for(int i=0;i<3;++i)
            for(int j=0;j<3;++j)
                N[i][j] = N4[i][j];
        set_uniform("N", N);
    }
    
    
    void ShaderProgramDraw::set_model_matrix(const CGLA::Mat4x4f& _model_matrix)
    {
        M = _model_matrix;
        set_uniform("M", M);
        set_derived_matrices();
    }
    
    void ShaderProgramDraw::set_view_matrix(const CGLA::Mat4x4f& _view_matrix)
    {
        V = _view_matrix;
        set_uniform("V", V);
        set_derived_matrices();
    }
    
    void ShaderProgramDraw::set_projection_matrix(const CGLA::Mat4x4f& _projection_matrix)
    {
        P = _projection_matrix;
        set_uniform("P", P);
        set_derived_matrices();
    }
    
    void ShaderProgramDraw::set_material(const CGLA::Vec4f& diff, const CGLA::Vec4f& spec, float exponent)
    {
        mat_diff = diff;
        mat_spec = spec;
        mat_spec_exp = exponent;

        set_uniform("mat_diff", mat_diff);
        set_uniform("mat_spec", mat_spec);
        set_uniform("mat_spec_exp", mat_spec_exp);
    }

    string generic_attribute_locations[] = {"vertex",
                   "normal",
                   "texcoord",
                   "dpdu",
                   "dpdv" };
    int no_generic_attribute_locations = sizeof(generic_attribute_locations) /  sizeof(generic_attribute_locations[0]);

    void ShaderProgramDraw::link(){
       for (int i=0;i< no_generic_attribute_locations;i++){
           glBindAttribLocation(prog, i, generic_attribute_locations[i].c_str());
        }
        ShaderProgram::link();
    }

    GLint ShaderProgramDraw::get_generic_attrib_location(const std::string& name)
    {
        for (int i=0;i<no_generic_attribute_locations;i++){
            if (name == generic_attribute_locations[i]) {
                return i;
            }
        }
        return -1;
    }

}

