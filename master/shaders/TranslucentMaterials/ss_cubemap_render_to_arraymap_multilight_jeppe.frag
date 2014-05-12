#version 430
#define RANDOM
#define TIME
#define MULTI_LIGHTS

layout(location = 0) out vec4 fragColor;


uniform sampler2DArray ntex;
uniform sampler2DArray vtex;
uniform sampler2D discpoints;
#ifdef TIME
uniform sampler2DArray colorMap;
#endif

smooth in vec3 position;
smooth in vec3 norm;

const int MAX_LIGHTS = 5;
uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_diff[MAX_LIGHTS];
uniform int light_number;
uniform mat4 lightMatrices[MAX_LIGHTS];

uniform float one_over_max_samples;
uniform float one_over_discs;

#ifdef TIME
uniform int convergence_frames;
uniform int current_frame;
const int DISCS = 10;
uniform mat4 cameraMatrices[DISCS];
uniform float epsilon_combination;
#endif

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

    float Ti = clamp(fresnel_T(_wi,_nin,1.0f,ior),0.0f,1.0f);
    //float To = fresnel_T(_wo,_no,1.0f,ior);


    _S *= Ti;// * To;
    _S = max(vec3(0.0f),_S);

    return _S;
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
    int layer = gl_Layer;
    vec3 xo = position;
    vec3 no = normalize(norm);

#ifdef TIME
    vec4 l = cameraMatrices[layer] * vec4(position,1.0f);
    vec4 oldColor = texture(colorMap,vec3(l.xy,layer));

    if(current_frame == 0)
    {
        oldColor = vec4(0.0f); //to avoid garbage on the first round
    }

    if(current_frame == convergence_frames)
    {
        fragColor = oldColor;
        return;
    }

#else
    vec4 oldColor = vec4(0.0f);
#endif

    vec3 color = vec3(0.0f);



    vec3 accumulate = vec3(0.0f);
    int i, count = 0;

#ifdef TIME
    float time = float(current_frame) / float(convergence_frames);
    int tt = current_frame;
//    tt = 0;
//    time = 0;
#else
    float time = 0;
    int tt = 0;
#endif


#ifdef RANDOM
    float noise1 = noise(xo * (layer+1) * (197));
    float noise2 = noise(xo * (layer+1) * (677 + tt));
    float r_angle = (noise1 + time) * 2 * M_PI;
    float delta_rad = discradius / samples * (noise2 - 0.5f);
    mat2 rot = mat2(cos(r_angle),sin(r_angle), -sin(r_angle), cos(r_angle));
#endif

    for(int k = 0; k < light_number; k++)
    {
        vec3 wi = vec3(light_pos[k]);
        wi = normalize(wi);

        vec3 offset = epsilon_gbuffer * (no - wi * dot(no,wi));
        vec3 position_mod = xo - offset;
        //vec3 position_mod = xo - epsilon * (1 - dot(no,wi)) * no;

        vec4 light_pos = lightMatrices[k] * vec4(position_mod,1.0f);
        vec2 circle_center = light_pos.xy;

        vec3 Li = light_diff[k].xyz;
        for(i = 0; i < samples; i++)
        {
    #ifdef RANDOM
            vec2 discoffset = (discradius + delta_rad) * rot * texture(discpoints,vec2(i * one_over_max_samples, layer * one_over_discs)).xy;
    #else
            vec2 discoffset = discradius * texture(discpoints,vec2(i * one_over_max_samples, layer * one_over_discs)).xy;
    #endif
            vec2 uvin = circle_center + discoffset;
            vec3 sampl = vec3(uvin, k);
            if(uvin.x >= 0.0f && uvin.x <= 1.0f && uvin.y >= 0.0f && uvin.y <= 1.0f)
            {
                vec3 xi = texture(vtex, sampl).rgb;
                if(xi.z > -990.0f)
                {
                    vec3 ni = texture(ntex, sampl).rgb;
                    vec3 S = bssrdf(xi,wi,ni,xo,no);
                    accumulate += Li * S;
                    count++;
                }
            }
        }
    }
    fragColor = oldColor + vec4(accumulate,1.0);
    //fragColor = vec4(noise1);
    //fragColor = texture(ntex, circle_center.xy);
    //fragColor = vec4(count-149,0.0,0.0,1.0);
    //fragColor = vec4(100 *abs(offset),1.0f);
   //fragColor = vec4(0,0,abs(dot(no,wi)),1.0f);
}
