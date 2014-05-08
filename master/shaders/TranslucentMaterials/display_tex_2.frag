#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;

out vec4 fragColor;
uniform float mipmap_LOD;

void main(void)
{
    int level = 6;
    fragColor = textureLod(colorMap,vec3(_tex.xy,level),mipmap_LOD);// + textureLod(colorMap,vec3(_tex.xy,level),1f);
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
    //fragColor = vec4(_tex,1.0);

}
