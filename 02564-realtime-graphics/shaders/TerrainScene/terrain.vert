#version 150
in vec3 vertex;
in vec3 normal;

uniform mat4 PVM;
uniform mat3 N;

out vec3 pos;
out vec3 norm;

void main()
{
        pos = vertex;
        norm = normalize(N * normal);
        gl_Position = PVM * vec4(vertex, 1);
}
