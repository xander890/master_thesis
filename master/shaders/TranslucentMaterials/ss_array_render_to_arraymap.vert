#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 M;

smooth out vec3 v_position;
smooth out vec3 v_norm;

void main()
{   vec4 pos = M * vec4(vec3(0,0,0) + vertex,1.0f);
    v_position = pos.xyz;
    v_norm = normalize(vec3(M * vec4(normal,0.0f)));
    gl_Position = pos;
}
