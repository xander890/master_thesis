#version 430
in vec3 vertex;
in vec3 normal;
in vec3 translucent;
in vec2 texcoord;

out vec3 _normal;
out vec2 _texcoord;
out vec3 _pos;
out vec3 _transl;

uniform vec4 light_pos[50];

uniform mat4 PVM;
uniform mat4 VM;
uniform mat4 M;
uniform mat3 N;

void main()
{
    _normal = normalize(vec3(M * vec4(normal,1.0f)));
    vec4 p = (M * vec4(vertex,1.0f));
    _pos = p.xyz;
    _transl = vec3(translucent);
    _texcoord = texcoord;
    gl_Position = PVM * vec4(vertex,1);
}
