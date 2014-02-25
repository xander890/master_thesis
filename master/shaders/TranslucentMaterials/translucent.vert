#version 150
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

#define SOLUTION_CODE

#ifdef SOLUTION_CODE
out vec3 _normal;
out vec3 _position;
#else
out vec4 _color;

uniform vec4 light_pos;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
#endif
out vec3 _texcoord;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;

void main()
{

    _normal = normalize(N*normal);
    _position = (VM * vec4(vertex,1)).xyz;
    _texcoord = texcoord;
    gl_Position = PVM * vec4(vertex,1);
}
