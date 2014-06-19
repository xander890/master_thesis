#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 93) out;

#include "ss_aincludes_constants.glinc"

uniform mat4 P;
uniform mat4 viewMatrices[DIRECTIONS];

void main(void)
{
    for(int i = 0; i < DIRECTIONS; i++)
    {
        gl_Layer = i;

        for(int k = 0; k < 3; k++)
        {
            vec4 v0 = gl_in[k].gl_Position;
            gl_Position = P * viewMatrices[i] * v0;
            EmitVertex();
        }

        EndPrimitive();
    }
}
