#version 430
smooth in vec3 pos;

uniform samplerCube skybox;

layout(location = 0) out vec4 fragColor;

void main()
{
    fragColor = texture(skybox,normalize(pos));
   // fragColor = vec4(pos,1.0)
}
