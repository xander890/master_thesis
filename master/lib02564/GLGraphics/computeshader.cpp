#include "computeshader.h"

using namespace GLGraphics;

void ComputeShader::relinquish()
{
    glDeleteProgram(prog);
    glDeleteShader(cs);
}

void ComputeShader::compile()
{
    prog = glCreateProgram();

    cs = load_a_shader(GL_COMPUTE_SHADER, shader_path,cs_file_name);

    if(cs) glAttachShader(prog, cs);
}
