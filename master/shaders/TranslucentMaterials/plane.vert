#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;

out vec3 _tex;

void main()
{
    _tex = texcoord;
    gl_Position = PVM * vec4(vertex,1);
}
