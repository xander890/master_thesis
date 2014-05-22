#version 430
in vec3 vertex;
in vec3 texcoord;
out vec3 v_tex;


void main()
{
    v_tex = texcoord;
    gl_Position = vec4(vertex.xy,0,1);
}
