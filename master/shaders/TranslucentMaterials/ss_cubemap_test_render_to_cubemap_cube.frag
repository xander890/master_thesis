#version 430
in vec3 _tex;
in vec3 _center;

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

out vec4 fragColor;

void main(void)
{
    fragColor = vec4(texture(depthCubemap, vec3(_tex.x,_tex.y,_tex.z)).r);
 //      fragColor = vec4(_tex,1.0);
}
