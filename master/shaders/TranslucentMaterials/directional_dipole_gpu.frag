#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit
uniform sampler2D vertices;
uniform sampler2D normals;
uniform sampler2D areas;
uniform int vertex_size;
uniform int vertex_tex_size;

// Shadows
uniform sampler2DShadow shadow;
uniform int shadowEnabled;
uniform mat4 Mat;


in vec3 _normal;
in vec3 _texcoord;
in vec3 _pos;

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

uniform float gamma;

const float EPSILON_MU = 0.0f;
const float M_PI = 3.141592654;

float sample_shadow_map(vec3 pos)
{
    vec4 light_pos = Mat * vec4(pos,1.0f);
    light_pos.z -= 0.0015; //bias to avoid shadow acne
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    return texture(shadow,light_pos.xyz).r;
}

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


vec3 S_finite(vec3 _xi,vec3 _nin,vec3 _wi,vec3 _xo, vec3 _no, vec3 _wo)
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

    if(dot_x_no < 0.0001f )
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
    float To = fresnel_T(_wo,_no,1.0f,ior);


    _S *= Ti;// * To;
    _S = max(vec3(0.0f),_S);

    return _S;
}


void main()
{
    vec3 no = normalize(_normal);
    vec3 wo = normalize(user_pos - _pos);
    vec3 xo = _pos;

    vec3 Lo = vec3(0.0f);

    float s = 1.0f / vertex_tex_size;
    vec3 Li_base = vec3(light_diff[0]);
    vec3 wi = normalize(light_pos[0].xyz);

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
            float visibility = dot_n_w;

            if(shadowEnabled > 0)
            {
                visibility = sample_shadow_map(xi);
            }

            if(visibility > 0.0f)
            {
                vec3 BSSRDF = S_finite(xi,ni,wi,xo,no,wo);
                Lo += Li_base * dot_n_w * BSSRDF * area;
            }
        }
    }

    fragColor = vec4(Lo,1.0f);
    fragColor = pow(fragColor, vec4(1.0f/gamma));
    //fragColor = vec4(sample_shadow_map(xo));
    //vec3 nx = texture(normals, _pos.xy).xyz;
    //vec3 nx1 = vec3(M * vec4(nx,0.0f));
    //fragColor = vec4(abs(nx1),1.0f);
}



