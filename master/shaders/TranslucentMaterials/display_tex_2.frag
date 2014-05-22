#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;
uniform sampler2DArray vtex;

out vec4 fragColor;
uniform float mipmap_LOD;

void main(void)
{
    int level = 4;
    fragColor = textureLod(colorMap,vec3(0.5 * _tex.xy + vec2(0.25),level ),mipmap_LOD);// + textureLod(colorMap,vec3(_tex.xy,level),1f);
    //fragColor = texture(vtex, vec3(_tex.xy,0));
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
   // fragColor = vec4(1.0);

}
