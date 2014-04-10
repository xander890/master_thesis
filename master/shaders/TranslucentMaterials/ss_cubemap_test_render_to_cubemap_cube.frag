#version 430
in vec3 _tex;
in vec3 _center;

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;
uniform float areacircle;

out vec4 fragColor;

void main(void)
{
    vec4 smp = texture(colorCubemap, vec3(_tex.x,_tex.y,_tex.z));
    fragColor = 1/300.0f * vec4(smp.xyz/smp.a,1.0);
    //fragColor = vec4(smp.xyz,1.0);
}
