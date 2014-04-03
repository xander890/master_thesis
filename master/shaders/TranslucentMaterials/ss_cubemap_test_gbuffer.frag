#version 430
in vec3 _tex;

uniform sampler2D vtex;
uniform sampler2D ntex;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(ntex,_tex.xy);
    //    fragColor = vec4(_tex,1.0);
}
