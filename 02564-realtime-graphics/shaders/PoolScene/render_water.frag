#version 150
#define SOLUTION_CODE

uniform sampler2D ref_tex;
uniform sampler2D hf_tex;
uniform sampler2D bg_tex;
uniform vec2 win_dim;

in vec3 pos;
in vec3 norm;
in vec2 _texcoord;

in vec3 u_tangent;
in vec3 v_tangent;

float sqr(float x){return x*x;}

float n_air = 1.000293;
float n_water = 1.3330;

#ifdef SOLUTION_CODE
vec2 dudv(vec2 tc)
{
    vec2 result;
    result.x = (texture(hf_tex, vec2(_texcoord.x+0.001, _texcoord.y)).x -
                texture(hf_tex, vec2(_texcoord.x-0.001, _texcoord.y)).x)/0.002;
    result.y = (texture(hf_tex, vec2(_texcoord.x, _texcoord.y+0.001)).x -
                texture(hf_tex, vec2(_texcoord.x, _texcoord.y-0.001)).x)/0.002;
    return result;
}
#endif

out vec4 fragColor;
void main()
{
    float F = 0.5;
    vec2 tc = gl_FragCoord.xy/win_dim;
#ifdef SOLUTION_CODE
    vec2 duv = 0.02 * dudv(_texcoord);
    //gl_FragColor = 0.6*texture(ref_tex, tc + duv) + 0.4*vec4(0.6,.8,1.0,0.  0);
    vec3 I = normalize(pos);
    vec3 N = normalize(norm - u_tangent * duv.x - v_tangent * duv.y);
    vec3 r = refract(-I, norm, n_air/n_water);
    float cos_theta_i = max(0.0,dot(-I, N));
    float cos_theta_t = dot(r, -N);
    float Rs = sqr((n_air*cos_theta_i-n_water*cos_theta_t)/(n_air*cos_theta_i+n_water*cos_theta_t));
    float Rp = sqr((n_air*cos_theta_t-n_water*cos_theta_i)/(n_air*cos_theta_t+n_water*cos_theta_i));
    F = sqr(sqr((1.0-cos_theta_i))); // ad hoc
    //    gl_FragColor = vec4(1.0,1.0,1.0,0) + vec4(0,0,0,1.0/F);
    F=(Rs+Rp)/2.0; // fresnel
    //       fragColor =  texture(hf_tex, tc);
    //    fragColor = vec4(_texcoord, 0.0,1.0);
    //	float R0 = sqr(n_water-n_air)/sqr(n_air+n_water);
    //	gl_FragColor.a = R0+(1.0-R0) * pow(1.0-cos_theta_i,5.0); // Schlick
    fragColor =  F*texture(ref_tex, tc + 0.1*duv)+(1.0-F)*(0.3*vec4(0.5,0.8,0.7,0)+0.7*texture(bg_tex, tc - 0.025*duv));
#else
    fragColor =  F*texture(ref_tex, tc)+(1.0-F)*(0.3*vec4(0.5,0.8,0.7,0)+0.7*texture(bg_tex, tc));
#endif
}
