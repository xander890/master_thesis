#version 430
layout(location = 0) out vec4 fragColor;
uniform sampler2D ntex;
uniform sampler2D vtex;
uniform sampler2D discpoints;

in vec3 position;
in vec3 norm;

uniform vec4 light_pos[50];
uniform vec4 light_diff[50];
uniform mat4 lightMatrix;


//const int DISC_POINTS = 300;
//uniform vec2 discpoints[DISC_POINTS];

uniform float one_over_max_samples;
uniform float one_over_discs;
uniform int currentDisc;


uniform float discradius;
uniform int samples;
uniform float epsilon_gbuffer;



uniform float ior;
uniform vec3 absorption;
uniform vec3 scattering;
uniform vec3 mean_cosine;
uniform vec3 D;
uniform float A;
uniform float C_s;
uniform float C_s_inv;
uniform float C_e;
uniform vec3 de;
uniform vec3 extinction;
uniform vec3 red_extinction;
uniform vec3 red_scattering;
uniform vec3 transmission;
uniform vec3 reduced_albedo;


const float EPSILON_MU = 0.0f;
const float M_PI = 3.141592654;

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


vec3 S_infinite(vec3 _x, vec3 _w12, vec3 _r, vec3 _no)
{
    vec3 _r_tr = transmission * _r;
    vec3 _r_tr_p1 = _r_tr + vec3(1.0f);
    float x_dot_w12 = dot(_x,_w12);
    float no_dot_w12 = dot(_no,_w12);
    float x_dot_no = dot(_x,_no);
    vec3 _D = D;
    vec3 _T = exp(-_r_tr);
    vec3 coeff = _T / pow(_r,vec3(3.0f));
    vec3 first = C_s * ( (_r * _r) / _D + 3 * _r_tr_p1 * x_dot_w12);
    vec3 second = C_e * ( 3 * _D * _r_tr_p1 * no_dot_w12 - (_r_tr_p1 + 3 * _D * (3 *_r_tr_p1 + _r_tr * _r_tr) / (_r * _r) * x_dot_w12) * x_dot_no);
    vec3 _S = coeff * (first - second);
    return _S;

}


vec3 bssrdf(vec3 _xi,vec3 _nin,vec3 _wi,vec3 _xo, vec3 _no)
{

    vec3 _x = _xo - _xi;
    float r_sqr = dot(_x,_x);

    vec3 _D = D;

    vec3 _w12 = refract2(_wi,_nin,1.0f,ior);

    float mu = -dot(_no, _w12);
    float dot_x_w12 = dot(_x,_w12);
    float dot_x_no = dot(_x,_no);

    vec3 _r_sqr = vec3(r_sqr);
    vec3 _dr_sqr = vec3(r_sqr);
    vec3 _de = de;


    if(mu > EPSILON_MU)
    {
        vec3 project = sqrt(vec3(r_sqr - dot_x_w12 * dot_x_w12)/sqrt(_r_sqr + _de*_de));
        vec3 _D_prime = abs(mu) * _D;
        _dr_sqr += _D_prime * (_D_prime + 2.0f * _de * project);
    }
    else
    {
        vec3 _D_prime = vec3(1.0f) / (3 * (extinction));
        _dr_sqr += _D_prime * _D_prime;
    }

    vec3 _dr = sqrt(_dr_sqr);
    vec3 _nistar;

    if(dot_x_no < 0.01f )
    {
        _nistar = normalize(_nin);
    }
    else
    {
        vec3 _t = normalize(cross(_nin,_x));
        _nistar = cross(normalize(_x),_t);
    }

    vec3 _xv = _xi + 2 * A * de * _nistar;
    vec3 _dv = vec3(length(_xo - _xv));
    vec3 _wv = _w12 - 2 * dot(_w12,_nistar) * _nistar;

    vec3 _S_r = S_infinite(_xo - _xi, _w12, _dr, _no);
    vec3 _S_v = S_infinite(_xo - _xv, _wv, _dv, _no);
    vec3 _S = _S_r - _S_v;

    float ex = 1.0f/(4 * C_s_inv) * 1.0f/(4.0f * M_PI * M_PI);
    _S *= ex;

    float Ti = fresnel_T(_wi,_nin,1.0f,ior);
    //float To = fresnel_T(_wo,_no,1.0f,ior);


    _S *= Ti;// * To;
    _S = max(vec3(0.0f),_S);

    return _S;
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

    vec3 accumulate = vec3(0.0f);
    int i, count = 0;


    for(i = 0; i < samples; i++)
    {

        vec2 discoffset = discradius * texture(discpoints,vec2(i * one_over_max_samples, currentDisc * one_over_discs)).xy;
        vec2 uvin = circle_center + discoffset;
        if(uvin.x >= 0.0f && uvin.x <= 1.0f && uvin.y >= 0.0f && uvin.y <= 1.0f)
        {
            vec3 xi = texture(vtex, uvin).rgb;
            if(xi.z > -990.0f)
            {
                vec3 ni = texture(ntex, uvin).rgb;
                vec3 S = bssrdf(xi,wi,ni,xo,no);
                accumulate += Li * S * dot(ni,wi);
                count++;
            }
        }
    }

    fragColor = vec4(accumulate,1.0f);

}
