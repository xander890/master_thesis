#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit

in vec3 _normal;
in vec3 _texcoord;
in vec3 _pos;
in vec3 _transl;

uniform vec4 light_pos[50];
uniform vec4 light_diff[50];
uniform vec4 light_spec[50];
uniform vec4 light_amb;

uniform mat4 M;
uniform mat4 VM;
uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
uniform float ior;
uniform vec3 user_pos;

out vec4 fragColor;

#define FRESNEL

vec3 refract2(vec3 inv, vec3 n, float n1, float n2)
{
    float eta = n1/n2;
    float c = dot(inv, n);
    return eta * (c * n - inv) - n * sqrt(max(0.0f,1 - eta * eta * (1 - c * c)));
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


void main()
{
    vec3 norm = normalize(_normal);

    fragColor = vec4(_transl,1.0);
#ifdef FRESNEL
    fragColor *= fresnel_T(normalize(user_pos - _pos), norm, 1.0f, ior);
#endif


}



