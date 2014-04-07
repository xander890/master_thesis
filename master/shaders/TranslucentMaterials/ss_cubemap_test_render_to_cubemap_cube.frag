#version 430
in vec3 _tex;
in vec3 _center;

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;
const float discarea = 50.0f;

out vec4 fragColor;

void main(void)
{
    vec4 smp = texture(colorCubemap, vec3(_tex.x,_tex.y,_tex.z));
    fragColor = vec4(smp.xyz,1.0f);
 //      fragColor = vec4(_tex,1.0);
}
