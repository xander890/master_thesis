#version 430
in vec3 _tex;


restrict uniform layout(rgba32f, binding = 0) image2DArray colorMap;


out vec4 fragColor;
uniform float mipmap_LOD;

void main(void)
{
    int level = 6;
    fragColor = imageLoad(colorMap,ivec3(ivec2(_tex.xy * 1024),level));// + textureLod(colorMap,vec3(_tex.xy,level),1f);
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
    //fragColor = vec4(_tex,1.0);

}
