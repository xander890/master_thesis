#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;
uniform sampler2DArray vtex;
uniform sampler2DArray ntex;
uniform sampler2D discpoints;

out vec4 fragColor;
uniform float mipmap_LOD;

#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_directional_bssrdf_opt.glinc"

void main(void)
{
    int level = 1;
    fragColor = vec4(0);

    vec3 xi = vec3(1,1,0);
    vec3 xo = vec3(2 * _tex.xy,0);
    vec3 ni = vec3(0,0,1);
    vec3 no = vec3(0,0,1);
    vec3 wi = normalize(vec3(0,0,1));
   //fragColor = vec4(bssrdf(xi,ni,wi,xo,no),1.0f) / 1;
    //fragColor = pow(fragColor,vec4(1/2.2f));
    for(int i = 0; i < 1; i++)
        fragColor += textureLod(colorMap,vec3(_tex.xy, 1),mipmap_LOD) / 10000000;// + textureLod(colorMap,vec3(_tex.xy,level),1f);
    //fragColor = (texture(ntex, vec3(_tex.xy,0)));
    //if(fragColor.z > 100)
    //    fragColor = vec4(0);
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
   // fragColor = vec4(1.0);

}
