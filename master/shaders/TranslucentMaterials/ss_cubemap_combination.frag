#version 430

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

out vec4 fragColor;

in vec3 pos;

void main(void)
{
    fragColor =
            vec4(texture(depthCubemap, vec3(pos.x,pos.y,10.0f)).r)
            //vec4(texture(depthCubemap, vec3(10.0,pos.y,pos.x)).r)


            ;
       //fragColor = vec4(0.0);
}
