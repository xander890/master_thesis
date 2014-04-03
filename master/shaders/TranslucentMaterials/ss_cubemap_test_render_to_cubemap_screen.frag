#version 430
in vec3 _tex;

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(depthCubemap, vec3(_tex.x,_tex.y,-1.0f));
    //    fragColor = vec4(_tex,1.0);
}
