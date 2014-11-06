#version 430
layout(location = 0) out vec4 fragColor;

smooth in vec3 position;
smooth in vec3 norm;

void main(void)
{
    fragColor = vec4(1.0f);
}
