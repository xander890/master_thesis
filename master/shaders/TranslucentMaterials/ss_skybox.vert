#version 430

in vec3 vertex;
in vec3 normal;

smooth out vec3 pos;

uniform mat4 PVM;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void main()
{
    gl_Position = PVM*vec4(vertex, 1);
    pos = (M * vec4(vertex, 1)).xyz;
}
