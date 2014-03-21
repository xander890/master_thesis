#version 430
in vec3 vertex;
uniform mat4 PVM;

void main()
{
    gl_Position = PVM * vec4(vertex, 1);
}

