#version 150
in vec3 vertex;
in vec3 normal;
in vec2 texcoord;
in vec3 dpdu;
in vec3 dpdv;
out vec2 tc;
void main()
{
    tc=texcoord;
    gl_Position = vec4(vertex,1);
}
