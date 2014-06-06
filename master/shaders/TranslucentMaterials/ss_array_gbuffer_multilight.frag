#version 430
smooth in vec3 position;
smooth in vec3 norm;

layout(location = 0) out vec4 pos;
layout(location = 1) out vec4 normal;

void main()
{
    pos.rgb = position;
    normal.rgb = normalize(norm);
}
