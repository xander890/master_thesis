#version 430
#define MAX_LAYERS 10

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

uniform int layers;

in vec3 v_tex[3];

out vec3 _tex;

void main(void)
{
    int l = layers;
    for(int i = 0; i < l; i++)
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
