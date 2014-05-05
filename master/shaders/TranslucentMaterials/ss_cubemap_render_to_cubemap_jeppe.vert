#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 M;

smooth out vec3 v_position;
smooth out vec3 v_norm;

void main()
{
    v_position = vec3(M * vec4(vertex,1));
    v_norm = normalize(vec3(M * vec4(normal,0)));
    gl_Position = M * vec4(vertex,1);
}
