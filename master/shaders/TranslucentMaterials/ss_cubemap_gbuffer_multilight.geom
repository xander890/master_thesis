#version 430
#define MAX_LIGHTS 4

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

uniform int layers;
uniform mat4 P;
uniform mat4 lightMatrices[MAX_LIGHTS];

smooth in vec3 v_pos[3];
smooth in vec3 v_norm[3];

smooth out vec3 position;
smooth out vec3 norm;

void main(void)
{
    int l = layers;
    for(int i = 0; i < l; i++)
    {
        gl_Layer = i;

        for(int k = 0; k < 3; k++)
        {
            vec4 v0 = gl_in[k].gl_Position;
            position = v_pos[k];
            norm = v_norm[k];
            gl_Position = P * lightMatrices[i] * v0;
            EmitVertex();
        }

        EndPrimitive();
    }
}
