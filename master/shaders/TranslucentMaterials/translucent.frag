#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit

#define SOLUTION_CODE

#ifdef SOLUTION_CODE
in vec3 _normal;
in vec3 _position;

uniform vec4 light_pos;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;



uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
#else
in vec4 _color;
#endif
in vec3 _texcoord;

out vec4 fragColor;

void main()
{

    vec3 norm = normalize(_normal);
    vec3 light_dir = normalize(light_pos.a > 0.0 ? light_pos.xyz - _position.xyz : light_pos.xyz);
    float cos_theta = max(dot(norm, light_dir), 0.0);

    // ambient and diffuse part
    vec4 color = mat_diff*(cos_theta*light_diff);

// specular part
//    vec3 refl_dir = reflect(normalize(_position.xyz), norm);
//    float r_dot_l = max(dot(refl_dir, light_dir), 0.0);
//    color += mat_spec*pow(r_dot_l, max(mat_spec_exp, 1.0))*light_spec;

    fragColor = color;

}
