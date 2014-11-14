#version 430
#define RR
uniform sampler2DArray colorMap;
uniform sampler2DArrayShadow depthMap;

uniform samplerRect skybox;
uniform int has_environment;

#include "ss_aincludes_constants.glinc"

uniform vec2 skybox_dim;

smooth in vec3 position;
smooth in vec3 norm;

out vec4 fragColor;

uniform vec3 user_pos;
uniform vec4 light_pos[50];
// epsilons (error control)
uniform float shadow_bias;
uniform float epsilon_combination;

uniform float one_over_max_samples;
uniform float total_area;
uniform float disc_area;
uniform float ior;
uniform float ior_sq;
uniform float t0;


uniform float mipmap_LOD;

uniform mat4 cameraMatrices[DIRECTIONS];
uniform vec3 camera_dirs[DIRECTIONS];

uniform float gamma;

uniform int current_frame;
uniform int samples;
uniform float global_coeff;

uniform float combined_uniform_coefficient;

const vec2 adj [5]= {
    vec2(0),
    0.5*ARRAY_TEX_STEP * vec2(-1.0f, +3.0f),
    0.5*ARRAY_TEX_STEP * vec2(+3.0f, +1.0f),
    0.5*ARRAY_TEX_STEP * vec2(+1.0f, -3.0f),
    0.5*ARRAY_TEX_STEP * vec2(-3.0f, +1.0f)
};

#include "ss_aincludes_optics.glinc"

vec2 vector_cubemap_to_uv(vec3 pos)
{
    vec3 p = normalize(pos);
    p.z = -p.z;
    p = p.yzx;
    return vec2(0.5f + 0.5f * atan(p.x,-p.z) * ONE_M_PI, acos(p.y) * ONE_M_PI);
}

float sample_shadow_map(vec4 light_pos)
{
    light_pos.w -= shadow_bias; //bias to avoid shadow acne
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    return texture(depthMap,light_pos).r;
}

vec4 sample_color_map(vec3 coord)
{
    return textureLod(colorMap, coord, mipmap_LOD);
}

void main(void)
{
    vec3 no = normalize(norm);
    vec3 wo = normalize(user_pos - position);

    fragColor = vec4(0.0f);
    float div = 0.0f;

    for(int i = 0; i < DIRECTIONS; i++)
    {
        vec3 dir = camera_dirs[i];
        vec3 offset = epsilon_combination * (no - dir * dot(no,dir));
        vec3 pos = position - offset;
        vec4 l = cameraMatrices[i] * vec4(pos,1.0f);

        float vis = 1.0f;
        for(int j = 0; j < 5; j++)
        {
            float offset = 1.0f;
            vec4 adjj = vec4(l.xy + offset * adj[j], i, l.z);
            float vi = sample_shadow_map(adjj.xyzw);
            vis *= vi;
        }

        vec4 color = sample_color_map(vec3(l.xy,i));
#ifdef RR
        fragColor += color * vis;
#else
        fragColor += (color.xyza / max(color.a,1)) * vis;
#endif
        div += vis;
    }


    fragColor /= max(div,1.0);

    float F = fresnel_T(wo,no);
    fragColor *= clamp(F,0.0f,1.0f);

#ifdef RR
    fragColor *= 1.0f/((current_frame));
#endif

    //if(div < 0.01)
    //if(false)
    //{
    //    vec3 refl = reflect(wo,no);
    //    vec4 refl_col = texture(skybox,vector_cubemap_to_uv(refl) * skybox_dim);
    //    fragColor += refl_col * (1 - F);
    //}

    //fragColor *= combined_uniform_coefficient / disc_area;

    //fragColor = pow(vec4(1) - exp(-fragColor), vec4(1.0/gamma));
   //fragColor = pow(fragColor , vec4(1.0/gamma));

   // fragColor = vec4((max(0,1-div)),0,0,1);

}
