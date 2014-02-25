#version 150
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;


void main()
{
    gl_Position = PVM * vec4(vertex,1);
}
