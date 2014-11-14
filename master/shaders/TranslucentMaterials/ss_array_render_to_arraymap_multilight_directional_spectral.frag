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
uniform mat4 worldToLightMatrices[MAX_LIGHTS];
uniform mat4 lightToWorldMatrices[MAX_LIGHTS];

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

vec3 derivative_ext (vec3 r, vec3 s, vec3 cosalpha)
{
  vec3 t1;
  vec3 t15;
  vec3 t2;
  vec3 t3;
  vec3 t4;
  vec3 t5;
  vec3 t6;
  vec3 t8;
  t1 = cosalpha;
  t2 = t1 * r;
  t3 = t1 * t1;
  t4 = r * r;
  t5 = t3 * t4;
  t6 = s * s;
  t8 = sqrt(t5 - t4 + t6);
  t15 = sqrt(2 * t2 * t8 - t4 + 2 * t5 + t6);
  return(s * (t2 + t8) / t15 / t8);
}

float qi(vec3 xi, vec3 ni, vec3 wi, vec3 xo)
{
    vec3 w = refract2(wi,ni);
    vec3 x = xi-xo;
    return length(x - w * dot(x,w));
}

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
        float light_type = 0;
        wi = (light_type > 0.0f)? normalize(topoint) : normalize(wi);
        vec3 Li = light_diff[current_light].xyz;
        Li = (light_type > 0.0f)? Li / dot(topoint,topoint) : Li;

        vec4 light_post = worldToLightMatrices[current_light] * vec4(xo,1.0f);
        vec2 circle_center = light_post.xy;
        vec3 circle_center_light = vec3(circle_center, current_light);
        vec3 surface_point =  texture(vtex, circle_center_light).rgb;
        vec3 surface_point_n =  texture(ntex, circle_center_light).rgb;

        //First seed based on the pixel is chosen
        float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y) + (current_frame+1) * samples;
        //float index_s = (layer+1)*(gl_FragCoord.x * ARRAY_TEX_SIZE + gl_FragCoord.y);

        uint seed_circle = tea(index_s,17.0f) ;

        vec3 lambda = 0.1*extinction;
        vec3 lambda_in = extinction.xyz;



        float initial_optim = length(bssrdf(surface_point,surface_point_n,wi,xo,no));
        vec2 cc = circle_center;

        for(int k = 0; k < 0; k++)
        {
            seed_circle = lcg(seed_circle);
            float zeta1 = seed_circle / 16777216.0f;
            seed_circle = lcg(seed_circle);
            float zeta2 = seed_circle / 16777216.0f;
            float phi = 2 * M_PI * zeta2;
            float radius = 0.1 * sqrt(zeta1);
            float radius_texture = radius * light_texture_to_world_factor;
            vec2 angles = vec2(cos(phi),sin(phi));
            vec2 new_center = circle_center + radius_texture * angles;
            vec3 sampl = vec3(new_center, current_light);
            if(sampl.x >= 0 && sampl.x <= 1 && sampl.y >= 0 && sampl.y<= 1)
            {
                vec3 xc = texture(vtex, sampl).rgb;
                vec3 nc = texture(ntex, sampl).rgb;
                float q = length(bssrdf(xc,nc,wi,xo,no));
                if(q > initial_optim)
                {
                    initial_optim = q;
                    cc = new_center;
                }
            }
        }
        //circle_center = cc;
        vec3 a = a_m/ lambda_in;


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

            vec3 refr = wi;
            vec3 pp = refr - dot(refr,no)*no;

            float param = b_m / 50;
            vec3 xo_r = xo + param*pp * 1/lambda.x;
            vec3 xo_g = xo + param*pp * 1/lambda.y;
            vec3 xo_b = xo + param*pp * 1/lambda.z;

            vec4 light_post_r = worldToLightMatrices[current_light] * vec4(xo_r,1.0f);
            vec4 light_post_g = worldToLightMatrices[current_light] * vec4(xo_g,1.0f);
            vec4 light_post_b = worldToLightMatrices[current_light] * vec4(xo_b,1.0f);

            vec3 sampl_r = vec3(light_post_r.xy + radius_texture.r * angles, current_light);
            vec3 sampl_g = vec3(light_post_g.xy + radius_texture.g * angles, current_light);
            vec3 sampl_b = vec3(light_post_b.xy + radius_texture.b * angles, current_light);

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
                vec3 dist = vec3(length(xo-xi_r),length(xo-xi_g),length(xo-xi_b));
                if(dist.x < 3.0f && dist.y < 3.0f && dist.z < 3.0f)
#endif
                {

                    vec3 ni_r = normalize(texture(ntex, sampl_r).rgb);
                    vec3 ni_g = normalize(texture(ntex, sampl_g).rgb);
                    vec3 ni_b = normalize(texture(ntex, sampl_b).rgb);


#ifdef EXPONENTIAL

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
                    vec3 x_cosines = vec3(
                        dot(normalize(xo-xi_r),ni_r),
                        dot(normalize(xo-xi_g),ni_g),
                        dot(normalize(xo-xi_b),ni_b));
                    vec3 alpha_cosines = clamp(sqrt(1 - clamp(x_cosines * x_cosines,0,1)),0.1,1.0);
                    vec3 standard_radius = vec3(length(surface_point - xo));
                    standard_radius = standard_radius.x > 3.0f? vec3(0) : standard_radius;

                    vec3 ws_r = (lightToWorldMatrices[current_light] * vec4(radius_texture.r * angles, 0 ,0)).xyz;
                    float len_r = length(ws_r - dot(ws_r,ni_r) * ni_r);
                    vec3 ws_g = (lightToWorldMatrices[current_light] * vec4(radius_texture.g * angles, 0 ,0)).xyz;
                    float len_g = length(ws_g - dot(ws_g,ni_g) * ni_g);
                    vec3 ws_b = (lightToWorldMatrices[current_light] * vec4(radius_texture.b * angles, 0 ,0)).xyz;
                    float len_b = length(ws_b - dot(ws_b,ni_b) * ni_b);



                    vec3 acc =  cosine *(S * normalization) * dist ;
                    accumulate +=acc;
                    //accumulate = vec3(len_r,len_g,len_b) * samples;
                    //accumulate = vec3(0,-initial_optim + qi(surface_point,surface_point_n,wi,xo),0) * 100;
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

    for(int j = 0; j < samples; j++)
    {
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

                vec3 dist = vec3(length(xo-xi_r),length(xo-xi_g),length(xo-xi_b));
                if(dist.x < 3.0f && dist.y < 3.0f && dist.z < 3.0f)
                {

                vec3 ni_r = texture(ntex, sampl_r).rgb;
                vec3 ni_g = texture(ntex, sampl_g).rgb;
                vec3 ni_b = texture(ntex, sampl_b).rgb;

                float r = bssrdf_spectral_part_r(xi_r,ni_r,wi,xo,no);
                float g = bssrdf_spectral_part_g(xi_g,ni_g,wi,xo,no);
                float b = bssrdf_spectral_part_b(xi_b,ni_b,wi,xo,no);
                vec3 S = vec3(r,g,b);

                    vec3 x_cosines = vec3(
                        dot(normalize(xo-xi_r),ni_r),
                        dot(normalize(xo-xi_g),ni_g),
                        dot(normalize(xo-xi_b),ni_b));
                    vec3 alpha_cosines = clamp(sqrt(1 - clamp(x_cosines * x_cosines,0,1)),0.1,1.0);

                 //uniform
                 vec3 cosine = vec3(dot(wi,ni_r),dot(wi,ni_g),dot(wi,ni_b));
#ifdef CENTRAL_DIFFERENCES
                accumulate += cosine * S *a*a*M_PI * samples * central_diff_rev;
#else
                accumulate +=cosine* S *a*a*M_PI;
#endif
                }
            }
    }
#endif

        accumulate *= Li;


    }

    fragColor = vec4(accumulate.xyz /(samples) , 1);

/*
    vec3 xoo = vec3( 0.047,0,0.1);
    vec3 noo = normalize(vec3(1,0,1));
    vec3 wi = normalize(vec3(0,0,1));

    float r = bssrdf_spectral_part_r(xo,no,wi,xoo,noo);
    float g = bssrdf_spectral_part_g(xo,no,wi,xoo,noo);
    float b = bssrdf_spectral_part_b(xo,no,wi,xoo,noo);
    vec3 S = vec3(r,g,b) *0.1;
    vec3 lambda = transmission;
    vec3 lambda_in = extinction;


    fragColor = vec4(S.xyz,1) + vec4(0.2);

    if(length(xoo-xo) < 0.005)
        fragColor = vec4(0,1,0,0);
    float param = 1.f - 0.0f;
    vec3 refr = wi;//normalize(refract2(wi,noo));
    vec3 xoor = xoo + param*(refr-dot(refr,noo)*noo) * 1/lambda.x;
    vec3 xoog = xoo + param*(refr-dot(refr,noo)*noo) * 1/lambda.y;
    vec3 xoob = xoo +param*(refr-dot(refr,noo)*noo) * 1/lambda.z;
    if(length(xoor-xo) < (1/lambda.x + 0.001) && length(xoor-xo) > (1/lambda.x - 0.001))
       fragColor = vec4(1,0,0,0);
    if(length(xoog-xo) < (1/lambda.y + 0.001) && length(xoog-xo) > (1/lambda.y - 0.001))
       fragColor = vec4(0,1,0,0);
    if(length(xoob-xo) < (1/lambda.z + 0.001) && length(xoob-xo) > (1/lambda.z - 0.001))
       fragColor = vec4(0,0,1,0);
*/
}
