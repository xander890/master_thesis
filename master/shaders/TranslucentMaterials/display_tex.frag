#version 430
in vec3 _tex;

#define FILTER 0

uniform sampler2DArray colorMap;

out vec4 fragColor;


uniform float texStep;
uniform int scaling;

uniform int currentLayer;

vec4 sampleTex(sampler2DArray smp, vec3 texcoord)
{
    return texture(smp,texcoord);
}

void main(void)
{
//    int currentLayer = gl_Layer;
#if FILTER == 0
    float adjStep = 1.0f /1024 * 2;
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

    float v0 = c0.a;
    float v1 = c1.a;
    float v2 = c2.a;
    float v3 = c3.a;
    float v4 = c4.a;
    float v5 = 0;
    float v6 = 0;
    float v7 = 0;
    float v8 = 0 ;
    float v9 = c9.a;
    float v10 = c10.a;
    float v11 = c11.a;
    float v12 = c12.a;

    vec4 step1 = c0 * v0 + c1 * v1 + c2 * v2 + c3 * v3 + c4 * v4;
    float vstep1 = v0 + v1 + v2 + v3 + v4;
    vec4 step2 = c5 * v5 + c6 * v6 + c7* v7 + c8 * v8;
    float vstep2 = v5 + v6 + v7 + v8;
    vec4 step3 = c9 * v9 + c10 * v10 + c11 * v11 + c12 * v12;
    float vstep3 = v9 + v10 + v11 + v12;

    fragColor = (step1 + step2 + step3 ) / (vstep1 + vstep2 + vstep3);

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
    fragColor = sampleTex(colorMap,vec3(_tex.xy,currentLayer));
#endif
    //fragColor = vec4(gl_FragCoord.x,gl_FragCoord.x,float(currentLayer)/1,1.0f);
    //fragColor = vec4(_tex,1.0);

    //if(length(texture(vtex,_tex.xy)) < discradius)


    //fragColor = vec4(float(currentLayer) / 10.0f);
}
