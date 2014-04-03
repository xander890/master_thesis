#version 430
in vec3 p;
in vec3 n;

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 normal;

void main()
{
    position.rgb = p;
    normal.rgb = normalize(n);
}
