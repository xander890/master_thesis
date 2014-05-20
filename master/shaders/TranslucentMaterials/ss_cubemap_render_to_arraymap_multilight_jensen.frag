#version 430
#define RANDOM
#define TIME
#define MULTI_LIGHTS

layout(location = 0) out vec4 fragColor;

#define DIRECTIONS 10
#define MAX_LIGHTS 5

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

#ifdef TIME
uniform int convergence_frames;
uniform int current_frame;

uniform mat4 cameraMatrices[DIRECTIONS];
uniform float epsilon_combination;
#endif

uniform float discradius;
uniform int samples;
uniform float epsilon_gbuffer;

const float M_PI = 3.141592654;

#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_jensen_bssrdf.glinc"

#include "ss_aincludes_random.glinc"


void main(void)
{
    int layer = gl_Layer;
    vec3 xo = position;
    vec3 no = normalize(norm);

#ifdef TIME
    vec4 l = cameraMatrices[layer] * vec4(position,1.0f);
    vec4 oldColor = texture(colorMap,vec3(l.xy,layer));

    if(current_frame == 0)
    {
        oldColor = vec4(0.0f); //to avoid garbage on the first round
    }

    if(current_frame == convergence_frames)
    {
        fragColor = oldColor;
        return;
    }

#else
    vec4 oldColor = vec4(0.0f);
#endif

    vec3 color = vec3(0.0f);



    vec3 accumulate = vec3(0.0f);
    int i, count = 0;

#ifdef TIME
    float time = float(current_frame) / float(convergence_frames);
    int tt = current_frame;
//    tt = 0;
//    time = 0;
#else
    float time = 0;
    int tt = 0;
#endif


#ifdef RANDOM
    float noise1 = noise(xo * (layer+1) * (197));
    float noise2 = noise(xo * (layer+1) * (677 + tt));
    float r_angle = (noise1 + time) * 2 * M_PI;
    float delta_rad = discradius / samples * (noise2 - 0.5f);
    mat2 rot = mat2(cos(r_angle),sin(r_angle), -sin(r_angle), cos(r_angle));

#endif

    for(int k = 0; k < light_number; k++)
    {
        vec3 wi = vec3(light_pos[k]);
        wi = normalize(wi);

        vec3 offset = epsilon_gbuffer * (no - wi * dot(no,wi));
        vec3 position_mod = xo - offset;
        //vec3 position_mod = xo - epsilon * (1 - dot(no,wi)) * no;

        vec4 light_pos = lightMatrices[k] * vec4(position_mod,1.0f);
        vec2 circle_center = light_pos.xy;

        vec3 Li = light_diff[k].xyz;
        for(i = 0; i < samples; i++)
        {
    #ifdef RANDOM
            vec2 discoffset = (discradius + delta_rad) * rot * texture(discpoints,vec2(i * one_over_max_samples, layer * one_over_discs)).xy;
    #else
            vec2 discoffset = discradius * texture(discpoints,vec2(i * one_over_max_samples, layer * one_over_discs)).xy;
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
                    accumulate += Li * S;
                    count++;
                }
            }
        }
    }

    fragColor = vec4(accumulate,1.0);

#ifdef TIME
    fragColor += oldColor;
#endif
    //fragColor = vec4(noise1);
    //fragColor = texture(ntex, circle_center.xy);
    //fragColor = vec4(count-149,0.0,0.0,1.0);
    //fragColor = vec4(100 *abs(offset),1.0f);
   //fragColor = vec4(0,0,abs(dot(no,wi)),1.0f);
}
