#version 150
#define SOLUTION_CODE
uniform sampler2D tex;  // Uniform specifying the texture unit
in vec3 _norm;
in vec2 _texcoord;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;

out vec4 fragColor;
void main()
{
    vec4 base_color = texture(tex, _texcoord)*mat_diff;
    vec4 color = base_color*0.5;

#ifdef SOLUTION_CODE

    vec3 norm = normalize(_norm);
    // Compute dot product of normal and light source direction
    float d = dot(norm,normalize(light_pos.xyz));

    // Note that if d<=0, no light from the light source can reach
    // the surface, so we do not add neither diffuse nor specular
    // contributions in that case.
    if(d>0.0)
    {
        color += base_color*d*light_diff;
        vec3 h = normalize(light_half_vec);
        float s = dot(norm, h);
        if(s>0.0)
            color += pow(s, mat_spec_exp)*
                    light_spec*mat_spec*base_color;
    }
#endif
    fragColor = color;
}
