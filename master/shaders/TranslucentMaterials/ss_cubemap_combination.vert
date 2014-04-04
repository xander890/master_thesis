#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;
uniform mat4 M;

out vec3 pos;

void main()
{
    pos = vertex;
    pos = mat3(M) * vertex;
    gl_Position = PVM * vec4(vertex,1);
}
