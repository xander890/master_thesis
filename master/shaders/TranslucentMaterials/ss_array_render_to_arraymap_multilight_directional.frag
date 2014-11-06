#version 430
#define RANDOM
#define TIME
#define RR
#define EXPONENTIAL
#define ON_GPU
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
uniform mat4 lightMatrices[MAX_LIGHTS];

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

#ifdef EXPONENTIAL
    #include "ss_aincludes_directional_bssrdf_correct.glinc"
#else
    //#include "ss_aincludes_directional_bssrdf_correct.glinc"
#endif

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

    float zeta = 0;
    float area_disc = point_radius * point_radius * M_PI;

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

        vec4 light_post = lightMatrices[current_light] * vec4(xo,1.0f);
        vec2 circle_center = light_post.xy;
        vec3 circle_center_light = vec3(circle_center, current_light);
        vec3 surface_point =  texture(vtex, circle_center_light).rgb;

        //First seed based on the pixel is chosen
#ifdef ON_GPU
        float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y) + (current_frame+1) * samples;
        //index_s = 0;
        uint first_seed_circle = tea(index_s,17.0f);
        uint second_seed_circle = tea(index_s,19.0f);
#endif

#ifdef RR
        uint seed = tea((layer+1) * gl_FragCoord.xy);
        int index = int(tea(vec2(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y,1.0f)));
        zeta = seed / 4294967296.0f;
#endif


        float lambda = min_tr;
        float max_red = min(red_extinction.x,min(red_extinction.y,red_extinction.z));
        float a = a_m / (max_red); //TODO PASS PARAMETERS FROM CPU

        vec2 points = vec2(0);
        float max_dist = 0;

        for(i = 0; i < samples; i++)
        {
            //Fetch the sample from the disc texture
#ifdef ON_GPU
            first_seed_circle = lcg(first_seed_circle);
            second_seed_circle = lcg(second_seed_circle);
            float zeta1 = first_seed_circle / 16777216.0f;
            float zeta2 = second_seed_circle / 16777216.0f;
    #ifdef RR
            float radius = point_radius * sqrt(zeta1);
    #else



            //float b = 1.41f;
            //float exp_l_b = 1.0f - exp(-(lambda) * b);
            //float exp_l_b_rev = 1.0f / exp_l_b;
            float max_radius = 1.0f/lambda;
#ifdef EXPONENTIAL
            //float radius = log(-1.0f/(exp(lambda * b) * zeta1 - zeta1 * exp(lambda * a) - exp(lambda * b))) / lambda + a + b;
            float radius = -log(1.0f - zeta1) / lambda + a;
//            float radius = -log(zeta1) / lambda;
#else
            float radius = max_radius * sqrt(zeta1);
#endif


    #endif
            float phi = 2 * M_PI * zeta2;

            vec2 smpl = vec2(radius * cos(phi), radius * sin(phi));
#else
            vec2 smpl = texelFetch(discpoints,(index + i + current_frame * samples) % (max_samples)).xy;
#endif

            vec3 sampl = vec3(circle_center + smpl * light_texture_to_world_factor , current_light);

            if(sampl.x >= 0.0f && sampl.x <= 1.0f && sampl.y >= 0.0f && sampl.y <= 1.0f)
            {
                vec3 xi = texture(vtex, sampl).rgb;
                float l = length(xo-xi);

#ifdef RR

                //float normalization = min_tr * l * exp(-(min_tr) * l);
                //float normalization = min_tr * l * exp(-(min_tr) * l);
                float normalization = exp(-(min_tr) * l);
                //float normalization;
                //if(distance < 0.005f)
                //   normalization = exp(-(min_tr) * l) * max(1.0f / (exponential_integral*pow(l,3.0f)), 1.0f);
                //else
                //normalization =  exp(-(min_tr) * l) / (exponential_integral*pow(l,3.0f));
                 //   normalization = exp(-(min_tr) * l);
                //float max_D = max(D.x,max(D.y,D.z));
                //vec3 _w12 = refract2(wi,ni);
                //float cosp = dot(_w12, normalize(xo-xi));
                //float normalization = max_D * min_tr * min_tr * 0.25 *
                //        (( l * l  * (1.0f/max_D + 3 * min_tr * l * cosp) + 3 * l * cosp)
                //           * exp(-1 * (min_tr * l)));
                //float normalization = D.x * min_tr * min_tr * 0.5

                if(zeta < normalization && xi.z > -990.0f)
                {

                    vec3 ni = texture(ntex, sampl).rgb;
                    vec3 S = bssrdf(xi,wi,ni,xo,no);
                    accepted++;
                    accumulate += S * Li / normalization;
                }
#else
                if(xi.z > -990.0f)
                {

                    vec3 ni = texture(ntex, sampl).rgb;

#ifdef EXPONENTIAL

                    float normalization = 2 * M_PI * (exp(-lambda * a)) / (lambda) * exp(lambda * radius) ;//exp((-lambda) * radius) * (lambda*(2*M_PI)) / (1 - exp(-lambda*1.41f));
                    vec3 S = bssrdf(xi,wi,ni,xo,no);
#else
                    float normalization = (M_PI * max_radius * max_radius);
                    vec3 S = bssrdf(xi,wi,ni,xo,no);
#endif
                    accepted++;

                    accumulate += dot(wi,ni)* (S * normalization);

                }
#endif
            }



#ifdef RR
            seed = lcg(seed);
            zeta = seed / 16777216.0f;
#endif

        }

#ifdef EXPONENTIAL
        float b = b_m/lambda ;//max_dist - length(points / max(accepted,1));
        float reduced_area = M_PI * (b * b - a * a);
        accumulate *= reduced_area;
        //if(current_frame == 0)
        {
            vec3 surface_point_normal =  texture(ntex, circle_center_light).rgb;
            //accumulate += samples * a*a*M_PI* max(dot(wi,no),0.0f) * bssrdf(surface_point,wi,surface_point_normal,xo,no);
        }


#endif
        accumulate *= Li;
    }

#ifdef RR
        fragColor = vec4(accumulate * area_disc,1.0f);
#else
        fragColor = vec4(accumulate.xyz,1.0f);

#endif

}
