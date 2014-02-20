#version 150
in vec3 vertex;
in vec3 normal;

uniform vec4 light_pos;
uniform mat4 PVM;
uniform mat3 N;
uniform float s;

out vec3 color;
out vec3 tc;

void main()
{
    gl_Position = PVM*vec4(vertex+0.15*s*normal,1);
    tc = vertex;
    color = vec3(.7,.5,.23)*(0.25+0.75*dot(light_pos.xyz,N*normal));
}
