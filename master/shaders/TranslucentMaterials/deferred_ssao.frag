#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex; 
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;
uniform vec3 disc_points[256];
uniform vec2 win_dim;

uniform int NO_DISC_POINTS;
const float RADIUS = 3.0;
const float WIN_RADIUS = 15.0;
const float WIN_SCALING = 2.0;

float sqr(float x) {return x*x;}
out vec4 fragColor;
#ifdef SOLUTION_CODE
void main()
{
	const float pi = 3.1415926;
        vec3 p = texture(gtex, WIN_SCALING*gl_FragCoord.xy).xyz;
	if(p.z == -1000.0)
                fragColor = vec4(1.0);
	else
	{
        vec3 n = texture(ntex, WIN_SCALING*gl_FragCoord.xy).xyz;
        float ambient_occlusion=0.0;
        float WR = 70.0*RADIUS/abs(p.z);
        for(int i=0;i<NO_DISC_POINTS;++i)
        {
            vec2 texcoord =  WIN_SCALING*gl_FragCoord.xy + WR*disc_points[i].xy;
            vec3 pos = texture(gtex,texcoord).xyz;
            vec3 H = pos - p.xyz;
            float h = length(H);
            vec3 v = H/h;
            ambient_occlusion += max(0.0,1.0 - sqr(h/RADIUS)) * max(0.0, dot(v,n));
        }
        fragColor = vec4(1.1-(1.0/NO_DISC_POINTS)*ambient_occlusion);
        }
}
#else
void main()
{
    const float pi = 3.1415926;
    vec3 p = texture(gtex, WIN_SCALING*gl_FragCoord.xy).xyz;
    if(p.z == -1000.0)
        fragColor = vec4(1.0);
    else
    {
        vec3 n = texture(ntex, WIN_SCALING*gl_FragCoord.xy).xyz;
        float ambient_occlusion=0.0;
        fragColor = vec4(.5,.5,.5,0);
    }
}
#endif
