#version 430

in vec3 vertex;
in vec3 normal;

out vec3 p;
out vec3 n;

uniform mat4 PVM;
uniform mat4 M;

void main()
{
    gl_Position = PVM * vec4(vertex, 1);
    vec4 pos = (M * vec4(vertex, 1));
    p = pos.xyz;
    n = (mat3(M) * normal);
}
