#version 430
in vec3 vertex;

uniform mat4 M;

void main()
{
    gl_Position = M * vec4(vertex, 1);
}
