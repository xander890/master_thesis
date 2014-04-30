#version 430
in vec3 _tex;

uniform sampler2DArray colorMap;

out vec4 fragColor;


void main(void)
{
    fragColor = texture(colorMap,vec3(_tex.xy,1)) / 100;
    //fragColor = vec4(_tex,1.0);

    //if(length(texture(vtex,_tex.xy)) < discradius)


    //    fragColor = vec4(_tex,1.0);
}
