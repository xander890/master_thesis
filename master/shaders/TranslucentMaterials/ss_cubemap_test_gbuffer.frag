#version 430
in vec3 _tex;

uniform sampler2D vtex;
uniform sampler2D ntex;

out vec4 fragColor;

const int DISC_POINTS = 150;
uniform vec2 discpoints[DISC_POINTS];
uniform float discradius;
uniform int samples;

void main(void)
{
    fragColor = texture(ntex,_tex.xy);

    //if(length(texture(vtex,_tex.xy)) < discradius)


    //    fragColor = vec4(_tex,1.0);
}
