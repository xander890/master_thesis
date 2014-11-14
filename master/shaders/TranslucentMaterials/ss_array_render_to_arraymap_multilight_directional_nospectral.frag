#version 430
#define EXPONENTIAL
#define CENTRAL_SAMPLE
layout(location = 0) out vec4 fragColor;

#include "ss_aincludes_constants.glinc"

uniform sampler2DArray ntex;
uniform sampler2DArray vtex;
uniform samplerBuffer discpoints;

smooth in vec3 position;
smooth in vec3 norm;

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_diff[MAX_LIGHTS];
uniform int light_number;
uniform mat4 worldToLightMatrices[MAX_LIGHTS];

uniform float one_over_max_samples;
uniform float one_over_discs;
uniform float min_tr;
uniform int max_samples;

uniform int convergence_frames;
uniform float current_frame_percentage;
uniform mat4 cameraMatrices[DIRECTIONS];
uniform int global_frame;
uniform int current_frame;

uniform float discradius;
uniform int samples;

uniform float a_m;
uniform float b_m;

#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_directional_bssrdf_correct.glinc"

#include "ss_aincludes_random.glinc"

uniform float point_radius;
uniform float light_texture_to_world_factor;

void main(void)
{

    int layer = gl_Layer;
    vec3 xo = position;
    vec3 no = normalize(norm);

    vec3 accumulate = vec3(0.0f);
    vec3 weight = vec3(0.0f);

    float time = current_frame_percentage;


#ifdef RANDOM
    //float noise = LCG(layer * gl_FragCoord.xy);//LFSR_Rand_Gen(layer * int(gl_FragCoord.x + ARRAY_TEX_SIZE * gl_FragCoord.y));
    //float r_angle = (noise + time + float(layer)/DIRECTIONS) * 2 * M_PI;
    //float c = cos(r_angle);
    //float s = sin(r_angle);
    //mat2 rotation_matrix = mat2(c,s,-s,c);
#endif

    int accepted = 0;
    int i = 0;

    float area_disc = point_radius * point_radius * M_PI;
    vec3 cosines = vec3(0.0f);
    float res = 0.0f;
    for(int k = 0; k < light_number; k++)
    {
        // sets up the light-dependent parameters.
        int current_light = k;
        vec3 wi = light_pos[current_light].xyz;
        vec4 rad = light_diff[current_light];
        vec3 topoint = wi - xo;
        float light_type = 0;
        wi = (light_type > 0.0f)? normalize(topoint) : normalize(wi);
        vec3 Li = light_diff[current_light].xyz;
        Li = (light_type > 0.0f)? Li / dot(topoint,topoint) : Li;

        float epsilon_gb = 0.001f;
        vec4 light_post = worldToLightMatrices[current_light] * vec4(xo,1.0f);
        vec4 light_post_corr = worldToLightMatrices[current_light] * vec4(xo - epsilon_gb * (no - wi * dot(no,wi)),1.0f);
        vec2 circle_center = light_post.xy;
        vec3 circle_center_light = vec3(circle_center, current_light);
        vec3 surface_point =  texture(vtex, vec3(light_post_corr.xy,current_light)).rgb;

        //First seed based on the pixel is chosen
        float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y) + (current_frame+1) * samples;
        //index_s = 0;
        uint first_seed_circle = tea(index_s,17.0f);
        uint second_seed_circle = tea(index_s,19.0f);
        res = length(xo-surface_point);
        float lambda = min_tr;
        float max_red = min(red_extinction.x,min(red_extinction.y,red_extinction.z));
        //float a = a_m / (max_red);
        float a = a_m / max_red;
        float b = b_m / min_tr;

        vec3 points = vec3(0);
        float max_dist = 0;

        for(i = 0; i < samples; i++)
        {
            //Fetch the sample from the disc texture
            first_seed_circle = lcg(first_seed_circle);
            second_seed_circle = lcg(second_seed_circle);
            float zeta1 = first_seed_circle / 16777216.0f;
            float zeta2 = second_seed_circle / 16777216.0f;

            //float b = 1.41f;
            //float exp_l_b = 1.0f - exp(-(lambda) * b);
            //float exp_l_b_rev = 1.0f / exp_l_b;
            float max_radius = 1.0f/lambda;

            //float radius = log(-1.0f/(exp(lambda * b) * zeta1 - zeta1 * exp(lambda * a) - exp(lambda * b))) / lambda + a + b;
            //float radius = -log(1.0f - zeta1) / lambda + a;

#ifdef EXPONENTIAL
            float radius = -log(1.0f - zeta1) / (lambda) + a;
#else
            float radius = a + (b-a) * sqrt(zeta1);
#endif
//            float radius = -log(zeta1) / lambda;


            float phi = 2 * M_PI * zeta2;
            float test = 1.0f;
            vec3 sampl = vec3(circle_center + vec2(radius * cos(phi), radius * sin(phi)) * light_texture_to_world_factor * test, current_light);

            if(sampl.x >= 0.0f && sampl.x <= 1.0f && sampl.y >= 0.0f && sampl.y <= 1.0f)
            {
                vec3 xi = texture(vtex, sampl).rgb;
                float l = length(xo-xi);

                if(xi.z > -990.0f)
                {

                    vec3 ni = texture(ntex, sampl).rgb;
#ifdef EXPONENTIAL
                    float normalization = 2 * M_PI * (exp(-lambda * a)) / (lambda )  * exp(lambda * radius) * l;
                    //normalization *=  (1 + lambda * a) / lambda;
#else
                    float normalization = 1 ;
#endif
                    vec3 S = bssrdf(xi,ni,wi,xo,no);

                    accepted++;
                    float cosine = dot(wi,ni);
                    accumulate += cosine * (S * normalization);
                    //points += xi;
                    //max_dist = max(max_dist, l);
                }

            }


        }

#ifdef EXPONENTIAL
        float reduced_area = M_PI * (b * b - a * a);

#else
        accumulate *= M_PI * b * b ;
#endif

#ifdef CENTRAL_SAMPLE
        first_seed_circle = lcg(first_seed_circle);
        second_seed_circle = lcg(second_seed_circle);
        float zeta1 = first_seed_circle / 16777216.0f;
        float zeta2 = second_seed_circle / 16777216.0f;
        float radius = a * sqrt(zeta1);
        float phi = 2 * M_PI * zeta2;

        vec3 sampl = vec3(circle_center + vec2(radius * cos(phi), radius * sin(phi)) * light_texture_to_world_factor, current_light);

        if(sampl.x >= 0.0f && sampl.x <= 1.0f && sampl.y >= 0.0f && sampl.y <= 1.0f)
        {
            vec3 xi = texture(vtex, sampl).rgb;


            if(xi.z > -990.0f)
            {

                vec3 ni = texture(ntex, sampl).rgb;
                vec3 S = bssrdf(xi,ni,wi,xo,no);
                accumulate += dot(wi,ni) * S *a*a*M_PI * accepted ; //uniform

            }
        }
#endif

        accumulate *= Li;


    }

    fragColor = vec4(accumulate.xyz ,accepted);
   // fragColor = vec4(10 * res.xxx,1);

}
