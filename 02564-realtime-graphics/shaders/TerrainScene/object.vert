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
uniform vec3 light_half_vec;
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
#ifdef SOLUTION_CODE
    _normal = normalize(N*normal);
    _position = (VM * vec4(vertex,1)).xyz;
#else
    vec3 norm = normalize(N*normal);
    vec3 position = (VM * vec4(vertex,1)).xyz;
    vec3 light_dir = normalize(light_pos.a > 0.0 ? light_pos.xyz - position : light_pos.xyz);
    float cos_theta = max(dot(norm, light_dir), 0.0);

    // ambient and diffuse part
    _color = mat_diff*(light_amb + cos_theta*light_diff);

    // specular part
    vec3 refl_dir = reflect(normalize(position.xyz), norm);
    float r_dot_l = max(dot(refl_dir, light_dir), 0.0);
    _color += mat_spec*pow(r_dot_l, max(mat_spec_exp, 1.0))*light_spec;

#endif
    _texcoord = texcoord;
    gl_Position = PVM * vec4(vertex,1);
}
