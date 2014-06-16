#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 60) out;

#include "ss_aincludes_constants.glinc"

uniform int layers;
uniform mat4 P;
uniform mat4 viewMatrices[DIRECTIONS];

smooth in vec3 v_position[3];
smooth in vec3 v_norm[3];

smooth out vec3 position;
smooth out vec3 norm;

void main(void)
{
    for(int i = 0; i < DIRECTIONS; i++)
    {
        gl_Layer = i;

        for(int k = 0; k < 3; k++)
        {
            vec4 v0 = gl_in[k].gl_Position;
            position = v_position[k];
            norm = v_norm[k];
            gl_Position = P * viewMatrices[i] * v0;
            EmitVertex();
        }

        EndPrimitive();
    }
}
