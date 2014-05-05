#version 430
in vec3 _tex;

uniform sampler2DArray colorMapX;

out vec4 fragColor;


void main(void)
{
    fragColor = texture(colorMapX,vec3(_tex.xy,0));
    //fragColor = vec4(0.0,0.0,1.0f,1.0);
    //fragColor = vec4(_tex,1.0);

}
