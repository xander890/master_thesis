#version 430
#define RANDOM
#define TIME
#define MULTI_LIGHTS
//#define OVERLAY
layout(early_fragment_tests) in;
layout(location = 0) out vec4 fragColor;

#include "ss_aincludes_constants.glinc"

uniform sampler2DArray ntex;
uniform sampler2DArray vtex;
uniform sampler2D discpoints;

#ifdef TIME
uniform sampler2DArray colorMap;
#endif

smooth in vec3 position;
smooth in vec3 norm;

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_diff[MAX_LIGHTS];
uniform int light_number;
uniform mat4 lightMatrices[MAX_LIGHTS];

uniform float one_over_max_samples;
uniform float one_over_discs;
uniform float min_tr;
uniform float max_samples;
#ifdef TIME
uniform int convergence_frames;
uniform float current_frame_percentage;
uniform mat4 cameraMatrices[DIRECTIONS];
uniform float epsilon_combination;
uniform int global_frame;
uniform int current_frame;
#endif

uniform float discradius;
uniform int samples;
uniform float epsilon_gbuffer;

#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_directional_bssrdf.glinc"

#include "ss_aincludes_random.glinc"

void main(void)
{

    int layer = gl_Layer;
    vec3 xo = position;


    vec3 no = normalize(norm);

#ifdef TIME
    vec4 l = cameraMatrices[layer] * vec4(position,1.0f);
    vec4 oldColor = current_frame == 0 ? vec4(0.0f) : texture(colorMap,vec3(l.xy,layer));

#else
    vec4 oldColor = vec4(0.0f);
#endif

    vec3 accumulate = vec3(0.0f);

#ifdef TIME
    float time = current_frame_percentage;
#else
    float time = 0;
#endif


#ifdef RANDOM
    vec3 s_1 = xo * 617 * (layer + 1);

    float noise = noise3(s_1);

    float r_angle = (noise + time) * 2 * M_PI;
    float delta_rad = discradius * one_over_max_samples * (noise - 0.5f);
    mat2 rot = mat2(cos(r_angle),sin(r_angle), -sin(r_angle), cos(r_angle));
#endif
    int count = 0;
#ifdef OVERLAY
    bool color  = false;
#endif
    for(int k = 0; k < light_number; k++)
    {
        vec3 wi = vec3(light_pos[k]);
        float li = length(vec3(light_pos[k]) - xo);
        wi = normalize(wi);

        vec3 offset = epsilon_gbuffer * (no - wi * dot(no,wi));
        vec3 position_mod = xo - offset;
        //vec3 position_mod = xo - epsilon * (1 - dot(no,wi)) * no;

        vec4 light_post = lightMatrices[k] * vec4(position_mod,1.0f);
        vec2 circle_center = light_post.xy;

        vec3 Li = light_diff[k].xyz;

        for(int i = 0; i < max_samples && count < samples; i++)
        {
            vec2 smpl = texture(discpoints,vec2(i * one_over_max_samples, layer * one_over_discs)).xy;
    #ifdef RANDOM

            vec2 discoffset = (discradius) * rot * smpl;
    #else
            vec2 discoffset = discradius * smpl;
    #endif
            vec2 uvin = circle_center + discoffset;
            vec3 sampl = vec3(uvin, k);

            if(uvin.x >= 0.0f && uvin.x <= 1.0f && uvin.y >= 0.0f && uvin.y <= 1.0f)
            {
                vec3 xi = texture(vtex, sampl).rgb;
                if(xi.z > -990.0f)
                {
                    vec3 ni = texture(ntex, sampl).rgb;
                    vec3 S = bssrdf(xi,wi,ni,xo,no);
                    float normalization = exp(-min_tr * length(discradius * smpl));
                    accumulate += Li * S * normalization;

                }
                count++;
            }
            #ifdef OVERLAY
                if(length(discradius * smpl + vec2(0.5) - gl_FragCoord.xy / 1024) < 5.f/1024)
                    color = true;
            #endif
        }

    }
#ifdef OVERLAY
    if(color)
        fragColor = vec4(1,0,0,0);
    else

        fragColor = vec4(1);
#else
        fragColor = vec4(accumulate,count) + vec4(oldColor.xyz,0);
#endif
    //fragColor = vec4(accumulate,count) + vec4(oldColor.xyz,0);
   // fragColor = vec4(count / float(samples));
}
