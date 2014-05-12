#version 430

in vec3 vertex;
in vec3 normal;

smooth out vec3 v_pos;
smooth out vec3 v_norm;

uniform mat4 PVM;
uniform mat4 M;

void main()
{
    gl_Position = M * vec4(vertex, 1);
    vec4 pos = (M * vec4(vertex, 1));
    v_pos = pos.xyz;
    v_norm = (mat3(M) * normal);
}
