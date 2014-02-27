#version 430
uniform sampler2D maintexture;

in vec3 _tex;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(maintexture,_tex.xy);
    //fragColor = vec4(_tex.xy,0.0,1.0);

}
