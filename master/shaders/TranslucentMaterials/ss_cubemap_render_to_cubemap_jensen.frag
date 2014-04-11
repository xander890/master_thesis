#version 430
layout(location = 0) out vec4 fragColor;
uniform sampler2D ntex;
uniform sampler2D vtex;

in vec3 position;
in vec3 norm;

uniform vec4 light_pos[50];
uniform vec4 light_diff[50];
uniform mat4 lightMatrix;

const int DISC_POINTS = 300;
uniform vec2 discpoints[DISC_POINTS];
uniform float discradius;
uniform int samples;
uniform float epsilon_gbuffer;

uniform float ior;
uniform vec3 red_extinction;
uniform vec3 D;
uniform vec3 transmission;
uniform vec3 reduced_albedo;

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


vec3 bssrdf(in vec3 xi, in vec3 wi, in vec3 ni, in vec3 xo, in vec3 no)
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
    //float To = fresnel_T(wo,no,nin,ntr);

    S = S *(1.0f/M_PI)* Ti;// * To;

    S = max(vec3(0.0f),S);
    //S = vec3(length(xo-xi));


    return S;
}

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
    return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                   mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}

void main(void)
{
    vec3 xo = position;
    vec3 no = normalize(norm);
    vec3 wi = vec3(light_pos[0]);
    wi = normalize(wi);

    vec3 offset = epsilon_gbuffer * (no - wi * dot(no,wi));
    vec3 position_mod = xo - offset;
    //vec3 position_mod = xo - epsilon * (1 - dot(no,wi)) * no;

    vec4 light_pos = lightMatrix * vec4(position_mod,1.0f);
    vec2 circle_center = light_pos.xy;

    vec3 color = vec3(0.0f);

    vec3 Li = light_diff[0].xyz;
    float r_angle = noise(xo * 1500) * 2 * M_PI;
    mat2 rot = mat2(cos(r_angle),sin(r_angle), -sin(r_angle), cos(r_angle));

    vec3 accumulate = vec3(0.0f);
    int i, count = 0;
    for(i = 0; i < samples; i++)
    {
        vec2 uvin = circle_center + discradius * (discpoints[i]);
        if(uvin.x >= 0.0f && uvin.x <= 1.0f && uvin.y >= 0.0f && uvin.y <= 1.0f)
        {
            vec3 xi = texture(vtex, uvin).rgb;
            if(xi.z > -990.0f)
            {
                vec3 ni = texture(ntex, uvin).rgb;
                vec3 S = bssrdf(xi,wi,ni,xo,no);
                accumulate += Li * dot(ni,wi) * S;
                count++;
            }
        }
    }

    fragColor = vec4(accumulate,1.0f);
    //fragColor = vec4(circle_center.x,circle_center.y,0,1.0f);


    //fragColor = texture(ntex, circle_center.xy);
    //fragColor = vec4(count-149,0.0,0.0,1.0);
    //fragColor = vec4(100 *abs(offset),1.0f);
   //fragColor = vec4(0,0,abs(dot(no,wi)),1.0f);
}
