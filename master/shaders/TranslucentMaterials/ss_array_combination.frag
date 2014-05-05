#version 430
#define TIME 0
#define DEBUG 0
uniform sampler2DArray colorMap;
uniform sampler2DArrayShadow depthMap;

smooth in vec3 position;
smooth in vec3 norm;

out vec4 fragColor;


uniform vec4 light_pos[50];
// epsilons (error control)
uniform float shadow_bias;
uniform float epsilon_combination;

uniform float one_over_max_samples;
uniform float total_area;
uniform float disc_area;

uniform float step_tex;
uniform float mipmap_LOD;

const int DIRECTIONS = 10;

uniform mat4 cameraMatrices[DIRECTIONS];

uniform float gamma;

uniform float current_frame_rev;

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
    vec3 offset = epsilon_combination * no;
    vec3 pos = position - offset;

    fragColor = vec4(0.0f);
    float div = 0.0f;
    for(int i = 0; i < DIRECTIONS; i++)
    {
        vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
        vec4 color = textureLod(colorMap,vec3(l.xy,i),mipmap_LOD);
        float vis = sample_shadow_map(l.xyz,i) * step(1.0f, color.a);
        fragColor += color * vis;
        div += vis;
    }

    fragColor /= div;

#if DEBUG == 1
    int i = 0;
    vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
    fragColor =  texture(colorMap,vec3(l.xy,i)) * vec4(sample_shadow_map(l.xyz,i));
#endif

    fragColor *= disc_area * one_over_max_samples;

#if TIME == 1
    fragColor *= current_frame_rev;
#endif
    fragColor = pow(fragColor, vec4(1/gamma));


}
