#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;
uniform sampler2DArray vtex;
uniform sampler2DArray ntex;
uniform sampler2DArrayShadow depthMap;
uniform sampler2DArrayShadow newDepthMap;
uniform samplerBuffer discpoints;
uniform sampler2D reference_tex;

uniform float current_frame_rev;
out vec4 fragColor;
uniform float mipmap_LOD;
uniform int points;
uniform float a_m;
uniform float b_m;

#include "ss_aincludes_constants.glinc"
#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_random.glinc"

#include "ss_aincludes_directional_bssrdf_correct.glinc"


void main(void)
{
    int level = 1;
    fragColor = vec4(0);
    fragColor = texture(reference_tex, _tex.xy);
    return;
    vec2 texcoord = 0.25 * _tex.xy + vec2(0.75 /2);
    vec3 xo = vec3(0,0,0);
    vec3 xi = vec3(2 * texcoord - vec2(1,1),0);
    vec3 ni = vec3(0,0,1);
    vec3 no = vec3(0,0,1);
    vec3 wi = normalize(vec3(0,0,1));

    float max_red = red_extinction.x; //min(red_extinction.x,min(red_extinction.y,red_extinction.z));
    vec3 bss = bssrdf(xi,ni,wi,xo,no);
    fragColor = vec4(bss,1.0f) ;
   // float a = a_m / (max_red);
    float lambda = transmission.x;// min(transmission.x,min(transmission.y,transmission.z));
    //float b = b_m / lambda;
    //fragColor = pow(fragColor,vec4(1/2.2f));

    //fragColor = vec4(texture(ntex,vec3(texcoord, 0)));
    //fragColor = vec4(float(lcg(lcg(tea(gl_FragCoord.x, gl_FragCoord.y))))/ 16777216.0f);

    //return;
    vec3 l = extinction;
    vec3 s = red_extinction;
    vec3 a = 0  / s;
    vec3 b = 1 / l;
    uint seed_circle = tea(vec2(0,17.0f));
    uint seed_circle_o = seed_circle;

    bool found = false;

    for(int i = 0; i < points; i++)
    {

        seed_circle = lcg(seed_circle);
        float zeta1 = seed_circle / 16777216.0f;
        seed_circle = lcg(seed_circle);
        float zeta2 = seed_circle / 16777216.0f;
        //float radius = 0.5* sqrt(zeta1);
        float phi = 2 * M_PI * zeta2;


        //float exp_l_b = 1.0f - exp(-(lambda) * b);
        //float exp_l_b_rev = 1.0f / exp_l_b;
//        float radius =  -log(zeta1) / lambda;



        float radius = log(-1.0f/(exp(l.x * b.x) * zeta1 - zeta1 * exp(l.x * a.x) - exp(l.x * b.x))) / l.x + a.x + b.x;
        // float radius = -log(1.0f - zeta1) / l.x + a.x;
        vec2 sam = vec2(radius * cos(phi), radius * sin(phi));
       // vec2 sam = vec2(x, y);

        if(length(sam/2 + (vec2(0.5f) - texcoord)) < 1.5f/512)
        {
            found = true;
        }
    }
    fragColor = found? vec4(1,0,0,0) : fragColor;
    found = false;

    seed_circle = seed_circle_o;


    for(int i = 0; i < points; i++)
    {

        seed_circle = lcg(seed_circle);
        float zeta1 = seed_circle / 16777216.0f;
        seed_circle = lcg(seed_circle);
        float zeta2 = seed_circle / 16777216.0f;
        //float radius = 0.5* sqrt(zeta1);
        float phi = 2 * M_PI * zeta2;


        //float exp_l_b = 1.0f - exp(-(lambda) * b);
        //float exp_l_b_rev = 1.0f / exp_l_b;
//        float radius =  -log(zeta1) / lambda;



         float radius = log(-1.0f/(exp(l.y * b.y) * zeta1 - zeta1 * exp(l.y * a.y) - exp(l.y * b.y))) / l.y + a.y + b.y;
         //float radius = -log(1.0f - zeta1) / l.y + a.y;
        vec2 sam = vec2(radius * cos(phi), radius * sin(phi));
       // vec2 sam = vec2(x, y);

        if(length(sam/2 + (vec2(0.5f) - texcoord)) < 1.5f/512)
        {
            found = true;
        }
    }
    fragColor = found? vec4(0,1,0,0) : fragColor;
    found = false;
    seed_circle = seed_circle_o;


    for(int i = 0; i < points; i++)
    {

        seed_circle = lcg(seed_circle);
        float zeta1 = seed_circle / 16777216.0f;
        seed_circle = lcg(seed_circle);
        float zeta2 = seed_circle / 16777216.0f;
        //float radius = 0.5* sqrt(zeta1);
        float phi = 2 * M_PI * zeta2;



        float radius = log(-1.0f/(exp(l.z * b.z) * zeta1 - zeta1 * exp(l.z * a.z) - exp(l.z * b.z))) / l.z + a.z + b.z;
        // float radius = -log(1.0f - zeta1) / l.z + a.z;
       vec2 sam = vec2(radius * cos(phi), radius * sin(phi));
        // vec2 sam = vec2(x, y);

        if(length(sam/2 + (vec2(0.5f) - texcoord)) < 1.5f/512)
        {
            found = true;
        }
    }
    fragColor = found? vec4(0,0,1,0) : fragColor;


    float radius = 2 * length((vec2(0.5f) - texcoord));
   // if(radius > a.x - 0.001 && radius < a.x + 0.001)
    //    fragColor = vec4(0,1,0,0);
    //if(radius > b.x - 0.001 && radius < b.x + 0.001)
   //     fragColor = vec4(0,1,0,0);
    vec3 x = vec3(texcoord,0.0f);
    vec3 n = vec3(0,0,1);

    //fragColor = vec4(bssrdf(x, n, normalize(n-x),n,n),1.0f);
    //fragColor = vec4(texcoordzz);
        //fragColor +=  (0.5 - 0.5* texture(newDepthMap,vec4(texcoord, 0, 0.5)) ).xxxx;// + textureLod(colorMap,vec3(texcoord,level),1f);
        //fragColor = noise(gl_FragCoord.xy).xxxx;

   // fragColor = LCG(uint(gl_FragCoord.x + 1000 * gl_FragCoord.y)).xxxx;
    //fragColor = vec4(1.0f);
    //fragColor = (texture(ntex, vec3(texcoord,0)));
    //if(fragColor.z > 100)
    //    fragColor = vec4(0);
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
    //fragColor = texture(discpoints,texcoord);



}
