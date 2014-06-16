#version 430
in vec3 _tex;

#define FILTER 0
#include "ss_aincludes_constants.glinc"

uniform sampler2DArray colorMap;

out vec4 fragColor;

uniform int scaling;

//uniform int currentLayer;

vec4 sampleTex(in sampler2DArray smp, in vec3 texcoord)
{
    return texture(smp,texcoord);
}

void main(void)
{
    int currentLayer = gl_Layer;
#if FILTER == 0
    float adjStep = ARRAY_TEX_STEP * 0.5 * scaling;
    vec4 c0 = sampleTex(colorMap,vec3(_tex.xy,currentLayer));
    vec4 c1 = sampleTex(colorMap,vec3(_tex.xy + vec2(adjStep,0.0f),currentLayer));
    vec4 c2 = sampleTex(colorMap,vec3(_tex.xy - vec2(adjStep,0.0f),currentLayer));
    vec4 c3 = sampleTex(colorMap,vec3(_tex.xy + vec2(0.0f, adjStep),currentLayer));
    vec4 c4 = sampleTex(colorMap,vec3(_tex.xy - vec2(0.0f, adjStep),currentLayer));

    vec4 c5 = sampleTex(colorMap,vec3(_tex.xy + 2 * vec2(adjStep,0.0f),currentLayer));
    vec4 c6 = sampleTex(colorMap,vec3(_tex.xy - 2 * vec2(adjStep,0.0f),currentLayer));
    vec4 c7 = sampleTex(colorMap,vec3(_tex.xy + 2 * vec2(0.0f, adjStep),currentLayer));
    vec4 c8 = sampleTex(colorMap,vec3(_tex.xy - 2 * vec2(0.0f, adjStep),currentLayer));

    vec4 c9 = sampleTex(colorMap,vec3(_tex.xy + vec2(adjStep,adjStep),currentLayer));
    vec4 c10 = sampleTex(colorMap,vec3(_tex.xy - vec2(adjStep,adjStep),currentLayer));
    vec4 c11 = sampleTex(colorMap,vec3(_tex.xy + vec2(adjStep, -adjStep),currentLayer));
    vec4 c12 = sampleTex(colorMap,vec3(_tex.xy - vec2(adjStep, -adjStep),currentLayer));

    float v0 = clamp(c0.a,0.0f,1.0f);
    float v1 = clamp(c1.a,0.0f,1.0f);
    float v2 = clamp(c2.a,0.0f,1.0f);
    float v3 = clamp(c3.a,0.0f,1.0f);
    float v4 = clamp(c4.a,0.0f,1.0f);
    float v5 = clamp(c5.a,0.0f,1.0f);
    float v6 = clamp(c6.a,0.0f,1.0f);
    float v7 = clamp(c7.a,0.0f,1.0f);
    float v8 = clamp(c8.a,0.0f,1.0f) ;
    float v9 = clamp(c9.a,0.0f,1.0f);
    float v10 = clamp(c10.a,0.0f,1.0f);
    float v11 = clamp(c11.a,0.0f,1.0f);
    float v12 = clamp(c12.a,0.0f,1.0f);

    vec4 step1 = c0 * v0 + c1 * v1 + c2 * v2 + c3 * v3 + c4 * v4;
    float vstep1 = v0 + v1 + v2 + v3 + v4;
    vec4 step2 = c5 * v5 + c6 * v6 + c7* v7 + c8 * v8;
    float vstep2 = v5 + v6 + v7 + v8;
    vec4 step3 = c9 * v9 + c10 * v10 + c11 * v11 + c12 * v12;
    float vstep3 = v9 + v10 + v11 + v12;

    fragColor = (step1 + step2 + step3 ) / max(vstep1 + vstep2 + vstep3, 1.0f);

    /*
    if(fragColor.r > 0 && fragColor.r < 14)
        fragColor = vec4(1,0,0,0);
    else
        fragColor = vec4(0);

    if(vstep1 + vstep2 + vstep3 > 0 && vstep1 + vstep2 + vstep3 < 10.1)
        fragColor += vec4(0,0,1,0);
    //fragColor = vec4(vstep1 + vstep2 + vstep3);
*/
#elif FILTER == 1
    vec4 c0 = sampleTex(colorMap,vec3(_tex.xy,currentLayer));
    vec4 c1 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,0.0f),currentLayer));
    vec4 c2 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,texStep),currentLayer));
    vec4 c3 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,0.0f),currentLayer));

    fragColor = 0.25 * vec4(c0 + c1 + c2 + c3);
#elif FILTER == 2
    vec4 c0 = sampleTex(colorMap,vec3(_tex.xy,currentLayer));
    vec4 c1 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,0.0f),currentLayer));
    vec4 c2 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,texStep),currentLayer));
    vec4 c3 = sampleTex(colorMap,vec3(_tex.xy + vec2(texStep,0.0f),currentLayer));

    float v0 = c0.a;
    float v1 = c1.a;
    float v2 = c2.a;
    float v3 = c3.a;

    fragColor = (c0 * v0 + c1 * v1 + c2 * v2 + c3 * v3) / (v0 + v1 + v2 + v3);
#else
    fragColor = sampleTex(colorMap,vec3(_tex.xy,currentLayer)) + vec4(0,0,0.3,0);
#endif
    //fragColor = vec4(gl_FragCoord.x,gl_FragCoord.x,float(currentLayer)/1,1.0f);
    //fragColor = vec4(_tex,1.0);

    //if(length(texture(vtex,_tex.xy)) < discradius)



}
