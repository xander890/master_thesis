#version 430
#define TIME 1
#define DEBUG 0
uniform sampler2DArray colorMap;
uniform sampler2DArrayShadow depthMap;

uniform samplerRect skybox;
uniform int has_environment;

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


uniform float step_tex;
uniform float mipmap_LOD;

const int DIRECTIONS = 10;
const float ONE_M_PI = 0.31830988618f;

uniform mat4 cameraMatrices[DIRECTIONS];
uniform vec3 camera_dirs[DIRECTIONS];

uniform float gamma;

uniform float current_frame_rev;

#include "ss_aincludes_optics.glinc"

vec2 vector_cubemap_to_uv(vec3 pos)
{
    vec3 p = normalize(pos);
    p.z = -p.z;
    p = p.yzx;
    return vec2(0.5f + 0.5f * atan(p.x,-p.z) * ONE_M_PI, acos(p.y) * ONE_M_PI);
}

float sample_shadow_map(vec3 light_pos, float layer)
{
    light_pos.z -= shadow_bias; //bias to avoid shadow acne
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    return texture(depthMap,vec4(light_pos.x,light_pos.y,layer,light_pos.z)).r;
}


void main(void)
{
    vec3 no = normalize(norm);
    vec3 wi = vec3(light_pos[0]);
    wi = normalize(wi);
    //vec3 offset = epsilon * (no - wi * dot(no,wi));

    vec3 wo = normalize(user_pos - position);

    fragColor = vec4(0.0f);
    float div = 0.0f;
    for(int i = 0; i < DIRECTIONS; i++)
    {
        vec3 dir = camera_dirs[i];
        vec3 offset = epsilon_combination * (no - dir * dot(no,dir));
        vec3 pos = position - offset;
        vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
        vec4 color = textureLod(colorMap,vec3(l.xy,i),mipmap_LOD);
        float vis = sample_shadow_map(l.xyz,i);
        fragColor += color * vis;
        div += vis;
    }

    fragColor /= div;

    float F = fresnel_T(wo,no,1.0f,ior);

#if DEBUG == 1
    int i = 1;
    vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
    fragColor = textureLod(colorMap,vec3(l.xy,i), 0) * vec4(sample_shadow_map(l.xyz,i));
#endif

    fragColor *= disc_area * one_over_max_samples;

#if TIME == 1
    fragColor *= current_frame_rev;
#endif

    //fragColor = vec4(div/2);
    //fragColor = vec4(0.0f);
    //if(div < 0.01)
    if(has_environment > 0)
    {
        vec3 refl = reflect(wo,no);
        vec4 refl_col = texture(skybox,vector_cubemap_to_uv(refl) * skybox_dim);
        fragColor += refl_col * (1 - F);
    }

    //fragColor = pow(fragColor, vec4(1/gamma));
}
