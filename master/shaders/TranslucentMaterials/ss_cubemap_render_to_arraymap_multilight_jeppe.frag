#version 430
#define RANDOM
#define TIME
#define MULTI_LIGHTS

layout(location = 0) out vec4 fragColor;

uniform sampler2DArray ntex;
uniform sampler2DArray vtex;
uniform sampler2D discpoints;

#ifdef TIME
uniform sampler2DArray colorMap;
#endif

smooth in vec3 position;
smooth in vec3 norm;

const int MAX_LIGHTS = 5;
uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_diff[MAX_LIGHTS];
uniform int light_number;
uniform mat4 lightMatrices[MAX_LIGHTS];

uniform float one_over_max_samples;
uniform float one_over_discs;

#ifdef TIME
uniform int convergence_frames;
uniform float current_frame_percentage;
const int DISCS = 10;
uniform mat4 cameraMatrices[DISCS];
uniform float epsilon_combination;
uniform int global_frame;
#endif

uniform float discradius;
uniform int samples;
uniform float epsilon_gbuffer;

const float M_PI = 3.141592654;

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
    vec4 oldColor = texture(colorMap,vec3(l.xy,layer)) * step(0.001f,current_frame_percentage);

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
    vec3 s_1 = xo * (layer+1) * (617 + global_frame);

    float noise = noise(s_1);

    float r_angle = (noise + time) * 2 * M_PI;
    float delta_rad = discradius * one_over_max_samples * (noise - 0.5f);
    mat2 rot = mat2(cos(r_angle),sin(r_angle), -sin(r_angle), cos(r_angle));
#endif

    for(int k = 0; k < light_number; k++)
    {
        vec3 wi = vec3(light_pos[k]);
        float li = length(vec3(light_pos[k]) - xo);
        wi = normalize(wi);

        vec3 offset = epsilon_gbuffer * (no - wi * dot(no,wi));
        vec3 position_mod = xo - offset;
        //vec3 position_mod = xo - epsilon * (1 - dot(no,wi)) * no;

        vec4 light_pos = lightMatrices[k] * vec4(position_mod,1.0f);
        vec2 circle_center = light_pos.xy;

        vec3 Li = light_diff[k].xyz  ;
        for(int i = 0; i < samples; i++)
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
                    //count++;
                }
            }
        }
    }
    fragColor = oldColor + vec4(accumulate,1.0);


}
