#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;
uniform mat4 M;

out vec3 position;
out vec3 norm;
uniform vec3 centerWorldCoordinates;

void main()
{
    position = vec3(M * vec4(vertex,1)) - centerWorldCoordinates;
    norm = vec3(M * vec4(normal,0));

    gl_Position = PVM * vec4(vertex,1);
}
