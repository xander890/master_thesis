#version 150
in vec3 vertex;
in vec2 texcoord;

out vec2 tc;

void main()
{
    tc=texcoord;
    gl_Position = vec4(vertex,1);
}
