#version 150
in vec3 vertex;
in vec3 normal;

#define SOLUTION_CODE

#ifdef SOLUTION_CODE
out vec3 _normal;
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

uniform mat4 PVM;
uniform mat3 N;

void main()
{
#ifdef SOLUTION_CODE
    _normal = normalize(N*normal);
#else
    vec3 norm = normalize(N*normal);
    float d = dot(norm,normalize(light_pos.xyz));

    _color = mat_diff*light_amb;
    if(d>0.0)
    {
        _color += mat_diff*d*light_diff;
        float n_dot_h = dot(norm, normalize(light_half_vec));
        if(n_dot_h>0)
            _color += mat_spec*pow(n_dot_h, mat_spec_exp)*light_spec;
    }
#endif

    gl_Position = PVM * vec4(vertex,1);
}
