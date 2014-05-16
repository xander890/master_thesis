#version 430
#define TIME 0
#define DEBUG 0
uniform sampler2DArray colorMap;
uniform sampler2DArrayShadow depthMap;

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

uniform mat4 cameraMatrices[DIRECTIONS];

uniform float gamma;

uniform float current_frame_rev;

vec3 refract2(vec3 inv, vec3 n, float n1, float n2)
{
    float eta = n1/n2;
    float c = dot(inv, n);
    return eta * (c * n - inv) - n * sqrt(1 - eta * eta * (1 - c * c));
}

vec2 fresnelAmplitudeTransmittance(vec3 inv, vec3 n, float n1, float n2)
{
    float cosin = dot(inv,n);
    vec3 refr = refract2(inv,n,n1,n2);
    float costr = dot(refr,-n);

    float t_s = (2 * n1 * cosin) / (n1 * cosin + n2 * costr);
    float t_p = (2 * n1 * cosin) / (n1 * costr + n2 * cosin);

    return vec2(t_s,t_p);
}


vec2 fresnelPowerTransmittance(vec3 inv, vec3 n, float n1, float n2)
{
    float cosin = dot(inv,n);
    vec3 refr = refract2(inv,n,n1,n2);
    float costr = dot(refr,-n);

    vec2 t = fresnelAmplitudeTransmittance(inv,n,n1,n2);

    return (n2 * costr) * ((t * t)/ (n1 * cosin));
}

float fresnel_T(vec3 inv, vec3 n, float n1, float n2)
{
    vec2 T = fresnelPowerTransmittance(inv,n,n1,n2);
    return 0.5 * (T.x + T.y);
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
    vec3 offset = epsilon_combination * no;
    vec3 pos = position - offset;

    vec3 wo = normalize(user_pos - pos);

    fragColor = vec4(0.0f);
    float div = 0.0f;
    for(int i = 0; i < DIRECTIONS; i++)
    {
        vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
        vec4 color = textureLod(colorMap,vec3(l.xy,i),3);
        float vis = sample_shadow_map(l.xyz,i) * step(1.0f, color.a);
        fragColor += color * vis;
        div += vis;
    }

    fragColor /= div;

    float F = clamp(fresnel_T(wo,no,1.0f,ior),0.0,1.0);

#if DEBUG == 1
    int i = 0;
    vec4 l = cameraMatrices[i] * vec4(pos,1.0f);
    fragColor =  textureLod(colorMap,vec3(l.xy,i),1) * vec4(sample_shadow_map(l.xyz,i));
#endif

    fragColor *= disc_area * one_over_max_samples * F;

#if TIME == 1
    fragColor *= current_frame_rev;
#endif
    fragColor = pow(fragColor, vec4(1/gamma));

    //fragColor = vec4(0.0f);
    //if(div < 0.01)
    //    fragColor = vec4(1.0f);
}
