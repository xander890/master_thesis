#version 150

uniform mat4 VM;
uniform mat4 PVM;
uniform mat3 N;

in vec3 vertex;
in vec3 normal;
in vec3 texcoord;
uniform vec3 dpdu;
uniform vec3 dpdv;

out vec3 u_tangent;
out vec3 v_tangent;
out vec2 _texcoord;

uniform vec2 win_dim;
out vec3 pos;
out vec3 norm;
void main()
{
    pos = (VM * vec4(vertex,1)).xyz;
    norm = N * normal;
    u_tangent = N*dpdu;
    v_tangent = N*dpdv;
    _texcoord = texcoord.xy;

    gl_Position = PVM * vec4(vertex, 1);
}
