#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include "ShaderProgram.h"

namespace GLGraphics {


    class ComputeShader : public ShaderProgram
    {
    public:

        GLuint cs;
        std::string cs_file_name;

        virtual void relinquish() override;

        virtual void compile() override;

        ComputeShader(const std::string& _shader_path,
                      const std::string& _cs_file_name):
        ShaderProgram(),
        cs(0),
        cs_file_name(_cs_file_name)
        {
            shader_path = _shader_path;
            reload();

        }

    };
}

#endif // COMPUTESHADER_H
