#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit
uniform sampler2D vertices;
uniform sampler2D normals;
uniform sampler2D areas;
uniform int vertex_size;
uniform int vertex_tex_size;

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

uniform vec3 user_pos;

out vec4 fragColor;

uniform float ior;
uniform vec3 red_extinction;
uniform vec3 D;
uniform vec3 transmission;
uniform vec3 reduced_albedo;

#define FRESNEL
const float M_PI = 3.141592654;

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


vec3 bssrdf(const vec3 xi, const vec3 wi, const vec3 ni, const vec3 xo, const vec3 wo, const vec3 no)
{
    float l = length(xo - xi);

    float ntr = ior;
    float nin = 1.0f; //air
    float eta = ntr / nin;
    float eta_sqr = eta * eta;
    float Fdr = -1.440 / eta_sqr + 0.71 / eta + 0.668 + 0.0636 * eta;
    float A = (1 + Fdr) / (1 - Fdr);

    vec3 zr = vec3(1.0f) / red_extinction;
    vec3 zv = zr + 4.0f * A * D;

    vec3 r = vec3(l);
    vec3 dr = sqrt(r * r + zr * zr);
    vec3 dv = sqrt(r * r + zv * zv);

    vec3 tr = transmission;
    vec3 C1 = zr * (tr + vec3(1.0f)/vec3(dr));
    vec3 C2 = zv * (tr + vec3(1.0f)/vec3(dv));

    vec3 coeff = reduced_albedo / (4.0f * M_PI);
    vec3 real = (C1 / (dr * dr)) * exp(- tr * dr);
    vec3 virt = (C2 / (dv * dv)) * exp(- tr * dv);

    vec3 S = coeff * (real + virt);
    float Ti = fresnel_T(wi,ni,nin,ntr);
    float To = fresnel_T(wo,no,nin,ntr);

    S *= (1.0f/M_PI)* Ti; //* To;

    S = max(vec3(0.0f),S);


    return S;
}


void main()
{
    vec3 no = normalize(_normal);
    vec3 wo = normalize(user_pos - _pos);
    vec3 xo = _pos;
    fragColor = vec4(_transl,1.0);
#ifdef FRESNEL
    //fragColor *= fresnel_T(normalize(user_pos - _pos), norm, 1.0f, ior);
#endif

    vec3 Lo = vec3(0.0f);

    float s = 1.0f / vertex_tex_size;
    vec3 Li_base = vec3(light_diff[0]);
    vec3 wi = normalize(vec3(M * vec4(light_pos[0].xyz, 0.0f)));

    for(int i = 0; i < vertex_tex_size; i++)
    {

        for(int j = 0; j < vertex_tex_size; j++)
        {
            if(j * vertex_tex_size + i >= vertex_size)
            {
                continue;
            }

            vec2 coord = vec2(j * s, i * s);
            vec3 ni = texture(normals, coord).xyz;
            ni = normalize(vec3(M * vec4(ni,0.0f)));
            vec3 xi = texture(vertices,coord).xyz;
            xi = vec3(M * vec4(xi,1.0f));
            float area = texture(areas, coord).x;

            float dot_n_w = dot(ni,wi);

            if(dot_n_w > 0.0f) //visibility term (for now)
            {
                vec3 BSSRDF = bssrdf(xi,wi,ni,xo,wo,no);
                Lo += Li_base * dot_n_w * BSSRDF * area;
            }
        }
    }

    fragColor = vec4(Lo,1.0f);
    //vec3 nx = texture(normals, _pos.xy).xyz;
    //vec3 nx1 = vec3(M * vec4(nx,0.0f));
    //fragColor = vec4(abs(nx1),1.0f);
}



