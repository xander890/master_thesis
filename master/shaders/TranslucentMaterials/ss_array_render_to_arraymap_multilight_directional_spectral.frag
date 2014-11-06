#version 430
#define EXPONENTIAL
#define CENTRAL_SAMPLE
//#define CENTRAL_DIFFERENCES
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

    for(int k = 0; k < 1; k++)
    {
        // sets up the light-dependent parameters.
        int current_light = k;
        vec3 wi = light_pos[current_light].xyz;
        vec4 rad = light_diff[current_light];
        vec3 topoint = wi - xo;
        float light_type = 1;
        wi = (light_type > 0.0f)? normalize(topoint) : normalize(wi);
        vec3 Li = light_diff[current_light].xyz;
        Li = (light_type > 0.0f)? Li / dot(topoint,topoint) : Li;

        vec4 light_post = lightMatrices[current_light] * vec4(xo,1.0f);
        vec2 circle_center = light_post.xy;
        vec3 circle_center_light = vec3(circle_center, current_light);
        vec3 surface_point =  texture(vtex, circle_center_light).rgb;

        //First seed based on the pixel is chosen
        float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y) + (current_frame+1) * samples;
        //float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y);

        uint seed_circle = tea(index_s,17.0f) ;

        vec3 lambda = transmission;
        vec3 lambda_in = extinction;

        vec3 a = a_m / lambda_in;
        vec3 b = b_m / lambda.xyz;
        vec3 points = vec3(0);
        float max_dist = 0;

        for(i = 0; i < samples; i++)
        {
            //Fetch the sample from the disc texture
            seed_circle = lcg(seed_circle);
            float zeta1 = seed_circle / 16777216.0f;
            seed_circle = lcg(seed_circle);
            float zeta2 = seed_circle / 16777216.0f;

            //float b = 1.41f;
            //float exp_l_b = 1.0f - exp(-(lambda) * b);
            //float exp_l_b_rev = 1.0f / exp_l_b;
            //float max_radius = 1.0f/lambda;

            //float radius = log(-1.0f/(exp(lambda * b) * zeta1 - zeta1 * exp(lambda * a) - exp(lambda * b))) / lambda + a + b;
            //float radius = -log(1.0f - zeta1) / lambda + a;

#ifdef EXPONENTIAL

            vec3 radius = -log(1.0f - zeta1) / (lambda.xyz) + a;

            // float radius = log(-1.0f/(exp(l.y * b.y) * zeta1 - zeta1 * exp(l.y * a.y) - exp(l.y * b.y))) / l.y + a.y + b.y;
#else
            vec3 radius = a + (b-a) * sqrt(zeta1);
#endif
//            float radius = -log(zeta1) / lambda;


            float phi = 2 * M_PI * zeta2;
            vec3 radius_texture = radius * light_texture_to_world_factor;
            vec2 angles = vec2(cos(phi),sin(phi));

            vec3 sampl_r = vec3(circle_center + radius_texture.r * angles, current_light);
            vec3 sampl_g = vec3(circle_center + radius_texture.g * angles, current_light);
            vec3 sampl_b = vec3(circle_center + radius_texture.b * angles, current_light);

            if(sampl_r.x >= 0.0f && sampl_r.x <= 1.0f && sampl_r.y >= 0.0f && sampl_r.y <= 1.0f
            && sampl_g.x >= 0.0f && sampl_g.x <= 1.0f && sampl_g.y >= 0.0f && sampl_g.y <= 1.0f
            && sampl_b.x >= 0.0f && sampl_b.x <= 1.0f && sampl_b.y >= 0.0f && sampl_b.y <= 1.0f)
            {
                vec3 xi_r = texture(vtex, sampl_r).rgb;
                vec3 xi_g = texture(vtex, sampl_g).rgb;
                vec3 xi_b = texture(vtex, sampl_b).rgb;

#ifdef CENTRAL_DIFFERENCES
                // For central differences
                float step_a = ARRAY_TEX_STEP;
                vec3 xi_r_minus = texture(vtex, sampl_r - vec3(angles * step_a,0)).rgb;
                vec3 xi_g_minus = texture(vtex, sampl_g - vec3(angles * step_a,0)).rgb;
                vec3 xi_b_minus = texture(vtex, sampl_b - vec3(angles * step_a,0)).rgb;

                vec3 xi_r_plus = texture(vtex, sampl_r + vec3(angles * step_a,0)).rgb;
                vec3 xi_g_plus = texture(vtex, sampl_g + vec3(angles * step_a,0)).rgb;
                vec3 xi_b_plus = texture(vtex, sampl_b + vec3(angles * step_a,0)).rgb;

                vec3 poi = xo;
                vec3 rad_minus = vec3(length(xi_r_minus - poi), length(xi_g_minus - poi), length(xi_b_minus - poi));
                vec3 rad_plus = vec3(length(xi_r_plus - poi), length(xi_g_plus - poi), length(xi_b_plus - poi));

                vec3 difff = (rad_plus - rad_minus) ;
                vec3 central_diff_rev = (step_a * 2) * clamp(1.0f/difff,-100,100);

                if(     xi_r.z > -990.0f && xi_g.z > -990.0f && xi_b.z > -990.0f &&
                        xi_r_plus.z > -990.0f && xi_g_plus.z > -990.0f && xi_b_plus.z > -990.0f &&
                        xi_r_minus.z > -990.0f && xi_g_minus.z > -990.0f && xi_b_minus.z > -990.0f
                )

#else
                if(xi_r.z > -990.0f && xi_g.z > -990.0f && xi_b.z > -990.0f)
#endif
                {

                    vec3 ni_r = texture(ntex, sampl_r).rgb;
                    vec3 ni_g = texture(ntex, sampl_g).rgb;
                    vec3 ni_b = texture(ntex, sampl_b).rgb;


#ifdef EXPONENTIAL
                    vec3 dist = vec3(length(xo-xi_r),length(xo-xi_g),length(xo-xi_b));
                    vec3 normalization = 2 * M_PI * (exp(-lambda * a)) / (lambda) * exp(lambda * radius)  ;
                   // normalization *=  (1 + lambda * a) / lambda;
#else
                    vec3 normalization = vec3(1) ;
#endif
                    float r = bssrdf_spectral_part_r(xi_r,ni_r,wi,xo,no);
                    float g = bssrdf_spectral_part_g(xi_g,ni_g,wi,xo,no);
                    float b = bssrdf_spectral_part_b(xi_b,ni_b,wi,xo,no);
                    vec3 S = vec3(r,g,b);

                    accepted++;
                    vec3 cosine = vec3(dot(wi,ni_r),dot(wi,ni_g),dot(wi,ni_b));

#ifdef CENTRAL_DIFFERENCES
                    accumulate += cosine* (S * normalization) * dist * central_diff_rev;
#else
                    accumulate += cosine* (S * normalization) * dist ;
#endif
                }
            }
        }

#ifdef EXPONENTIAL
        //float reduced_area = M_PI * (b * b - a * a);
        //accumulate *= reduced_area;
#else
        //accumulate *= M_PI * b * b;
#endif

#ifdef CENTRAL_SAMPLE
    // secondary sample count
    //int M = 5;
    //accumulate *= M;
    //for(int j = 0; j <  M; j++)
    //{
        seed_circle = lcg(seed_circle);
        float zeta1 = seed_circle / 16777216.0f;
        seed_circle = lcg(seed_circle);
        float zeta2 = seed_circle / 16777216.0f;
        vec3 radius = a * sqrt(zeta1);
        float phi = 2 * M_PI * zeta2;
        vec3 radius_texture = radius * light_texture_to_world_factor;
        vec2 angles = vec2(cos(phi),sin(phi));

        vec3 sampl_r = vec3(circle_center + radius_texture.r * angles, current_light);
        vec3 sampl_g = vec3(circle_center + radius_texture.g * angles, current_light);
        vec3 sampl_b = vec3(circle_center + radius_texture.b * angles, current_light);

        if(sampl_r.x >= 0.0f && sampl_r.x <= 1.0f && sampl_r.y >= 0.0f && sampl_r.y <= 1.0f
        && sampl_g.x >= 0.0f && sampl_g.x <= 1.0f && sampl_g.y >= 0.0f && sampl_g.y <= 1.0f
        && sampl_b.x >= 0.0f && sampl_b.x <= 1.0f && sampl_b.y >= 0.0f && sampl_b.y <= 1.0f)
        {
            vec3 xi_r = texture(vtex, sampl_r).rgb;
            vec3 xi_g = texture(vtex, sampl_g).rgb;
            vec3 xi_b = texture(vtex, sampl_b).rgb;

            //float l = length(xo-xi);

#ifdef CENTRAL_DIFFERENCES
                // For central differences
                float step_a = ARRAY_TEX_STEP;
                vec3 xi_r_minus = texture(vtex, sampl_r - vec3(angles * step_a,0)).rgb;
                vec3 xi_g_minus = texture(vtex, sampl_g - vec3(angles * step_a,0)).rgb;
                vec3 xi_b_minus = texture(vtex, sampl_b - vec3(angles * step_a,0)).rgb;

                vec3 xi_r_plus = texture(vtex, sampl_r + vec3(angles * step_a,0)).rgb;
                vec3 xi_g_plus = texture(vtex, sampl_g + vec3(angles * step_a,0)).rgb;
                vec3 xi_b_plus = texture(vtex, sampl_b + vec3(angles * step_a,0)).rgb;

                vec3 poi = xo;
                vec3 rad_minus = vec3(length(xi_r_minus - poi), length(xi_g_minus - poi), length(xi_b_minus - poi));
                vec3 rad_plus = vec3(length(xi_r_plus - poi), length(xi_g_plus - poi), length(xi_b_plus - poi));

                vec3 difff = (rad_plus - rad_minus) ;
                vec3 central_diff_rev = (step_a * 2) * clamp(1.0f/difff,-100,100);
#endif

            if(xi_r.z > -990.0f && xi_g.z > -990.0f && xi_b.z > -990.0f)
            {

                vec3 ni_r = texture(ntex, sampl_r).rgb;
                vec3 ni_g = texture(ntex, sampl_g).rgb;
                vec3 ni_b = texture(ntex, sampl_b).rgb;

                float r = bssrdf_spectral_part_r(xi_r,ni_r,wi,xo,no);
                float g = bssrdf_spectral_part_g(xi_g,ni_g,wi,xo,no);
                float b = bssrdf_spectral_part_b(xi_b,ni_b,wi,xo,no);
                vec3 S = vec3(r,g,b);

                 //uniform
#ifdef CENTRAL_DIFFERENCES
                accumulate += vec3(dot(wi,ni_r),dot(wi,ni_g),dot(wi,ni_b))* S *a*a*M_PI * samples * central_diff_rev;
#else
                accumulate += vec3(dot(wi,ni_r),dot(wi,ni_g),dot(wi,ni_b))* S *a*a*M_PI * samples;
#endif
            }
        }
    //}
#endif

        accumulate *= Li;


    }

    fragColor = vec4(accumulate.xyz , 1);

/*
    vec3 xoo = vec3(0.603669,-0.0027097,-0.0331928);
    vec3 noo = normalize(vec3(1,-1,0));
    vec3 wi = vec3(0,0,1);
    float r = bssrdf_spectral_part_r(xo,no,wi,xoo,noo);
    float g = bssrdf_spectral_part_g(xo,no,wi,xoo,noo);
    float b = bssrdf_spectral_part_b(xo,no,wi,xoo,noo);
    vec3 S = vec3(r,g,b);
    vec3 lambda = transmission;
    vec3 lambda_in = extinction;


    fragColor = vec4(S,1) + vec4(0.2);

    if(length(xoo-xo) < 1 / lambda_in.y)
        fragColor = vec4(0,0,1,0);

    if(length(xoo-xo) < (1/lambda.x + 0.001) && length(xoo-xo) > (1/lambda.x - 0.001))
        fragColor = vec4(1,0,0,0);
    if(length(xoo-xo) < (1/lambda.y + 0.001) && length(xoo-xo) > (1/lambda.y - 0.001))
        fragColor = vec4(0,1,0,0);
    if(length(xoo-xo) < (1/lambda.z + 0.001) && length(xoo-xo) > (1/lambda.z - 0.001))
        fragColor = vec4(0,0,1,0);
*/
}
