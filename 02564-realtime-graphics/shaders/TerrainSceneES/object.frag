precision mediump float;

uniform sampler2D tex;  // Uniform specifying the texture unit

#define SOLUTION_CODE

#ifdef SOLUTION_CODE
varying vec3 _normal;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
#else
varying vec4 _color;
#endif
varying vec3 _texcoord;

void main()
{
    vec4 base_color = texture2D(tex, _texcoord.xy);

#ifdef SOLUTION_CODE
    vec3 norm = normalize(_normal);
    // Compute dot product of normal and light source direction
    float d = dot(norm,normalize(light_pos.xyz));
    vec4 color = base_color*light_amb;
    // Note that if d<=0, no light from the light source can reach
    // the surface, so we do not add neither diffuse nor specular
    // contributions in that case.
    if(d>0.0)
    {
            color += base_color*d*light_diff;
            vec3 h = normalize(light_half_vec);
            float n_dot_h = dot(norm, normalize(light_half_vec));
            if (n_dot_h > 0.0)
                color += mat_spec*pow(n_dot_h, mat_spec_exp)*light_spec;
    }
    gl_FragColor = color;
#else
    gl_FragColor = base_color * _color;
#endif
}
