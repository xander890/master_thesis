#version 150
uniform sampler3D noisetex;
in vec3 color;
in vec3 tc;
uniform float s;
out vec4 fragColor;
void main(void)
{
    fragColor.rgb = color*(0.35+0.45*s);
    fragColor.rgb += vec3(0.2,.1,.05)*texture(noisetex, 4*tc).rgb;
    fragColor.a = 0.25*smoothstep(s-.4,s-.35,texture(noisetex, 12*tc).r);
}
