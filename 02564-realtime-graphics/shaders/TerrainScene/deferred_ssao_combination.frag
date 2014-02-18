#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex;
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;
uniform sampler2DRect ssao_tex;
uniform sampler2DShadow	shadow_map;
//uniform sampler2D	shadow_map;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform mat4 Mat;
const float INV_WIN_SCALING = 0.5;

float sample_shadow_map(vec3 eye_pos)
{
#ifdef SOLUTION_CODE
    vec4 ep;
    ep.xyz = eye_pos;
    ep.w = 1.0;
    vec4 light_pos =  Mat * ep;
    light_pos.z -= 0.0015;
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    return texture(shadow_map, light_pos.xyz);
#else
    return 1.0;
#endif
}

out vec4 fragColor;
void main()
{
#ifdef SOLUTION_CODE
    vec3 p = texture(gtex, gl_FragCoord.xy).xyz;
    vec4 base_color = texture(ctex, gl_FragCoord.xy);
    if(p.z == -1000.0)
        fragColor  = vec4(0.4,0.35,0.95,0);
    else
    {
        vec3 norm = normalize(texture(ntex, gl_FragCoord.xy).xyz);

        // Compute ambient light (including SSAO scaling)
        vec4 light_info = texture(ssao_tex, INV_WIN_SCALING*(gl_FragCoord.xy));
        vec4 color = base_color * light_amb  * light_info.r;

        // Direct light computation
        vec3 light_dir = normalize(light_pos.a > 0.0 ? light_pos.xyz - p : light_pos.xyz);
        float cos_theta = dot(norm, light_dir);
        if(cos_theta>0) {
            // Compute diffuse contribution
            float direct_light = sample_shadow_map(p);
            color += base_color*direct_light*cos_theta*light_diff;

            // Compute specular
            vec3 refl_dir = reflect(normalize(p), norm);
            float r_dot_l = max(dot(refl_dir, light_dir), 0.0);
            color += base_color.w * pow(r_dot_l, 10) * light_spec * direct_light;
        }
        fragColor = color;
    }
#else
    vec3 p = texture(gtex, gl_FragCoord.xy).xyz;
    vec4 base_color = texture(ctex, gl_FragCoord.xy);
    if(p.z == -1000.0)
        fragColor  = vec4(0.4,0.35,0.95,0);
    else
        fragColor = texture(ctex, gl_FragCoord.xy);
#endif
}
