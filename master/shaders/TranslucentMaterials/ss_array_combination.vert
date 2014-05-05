#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;
uniform mat4 M;

smooth out vec3 position;
smooth out vec3 norm;

void main()
{
    position = vec3(M * vec4(vertex,1.0f));
    norm = vec3(M * vec4(normal,0.0f));
    gl_Position = PVM * vec4(vertex,1.0f);
}
