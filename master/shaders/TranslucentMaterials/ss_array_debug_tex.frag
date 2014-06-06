#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;
uniform sampler2DArray vtex;
uniform sampler2D discpoints;

out vec4 fragColor;
uniform float mipmap_LOD;

void main(void)
{
    int level = 1;
    fragColor = vec4(0);
    for(int i = 0; i < 1; i++)
        fragColor += textureLod(colorMap,vec3(_tex.xy, 1),mipmap_LOD);// + textureLod(colorMap,vec3(_tex.xy,level),1f);
   // fragColor = 20 * texture(vtex, vec3(_tex.xy,0));
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
   // fragColor = vec4(1.0);

}
