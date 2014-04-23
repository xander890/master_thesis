#version 430
in vec3 _tex;

uniform sampler2D discpoints;

out vec4 fragColor;


void main(void)
{
    fragColor = texture(discpoints,1.5 * _tex.xy);
    //fragColor = vec4(_tex,1.0);

    //if(length(texture(vtex,_tex.xy)) < discradius)


    //    fragColor = vec4(_tex,1.0);
}
