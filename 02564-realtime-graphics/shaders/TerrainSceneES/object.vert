attribute vec3 vertex;
attribute vec3 normal;
attribute vec3 texcoord;

#define SOLUTION_CODE


#ifdef SOLUTION_CODE
varying vec3 _normal;
#else
varying vec4 _color;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
#endif
varying vec3 _texcoord;

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
        if(n_dot_h>0.0)
            _color += mat_spec*pow(n_dot_h, mat_spec_exp)*light_spec;
    }
#endif
        _texcoord = texcoord;
        gl_Position = PVM * vec4(vertex, 1);
}
