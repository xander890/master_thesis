#version 430

#include "ss_aincludes_constants.glinc"

layout(triangles) in;
layout(triangle_strip, max_vertices = 93) out;

in vec3 v_tex[3];

out vec3 _tex;

void main(void)
{

    for(int i = 0; i < DIRECTIONS; i++)
    {
        gl_Layer = i;

        for(int k = 0; k < 3; k++)
        {
            vec4 v = gl_in[k].gl_Position;
            _tex = v_tex[k];
            gl_Position = v;
            EmitVertex();
        }

        EndPrimitive();
    }
}
