#version 430
smooth in vec3 position;
smooth in vec3 norm;

layout(location = 0) out vec3 pos;
layout(location = 1) out vec3 normal;

void main()
{
    pos.rgb = position;
    normal.rgb = normalize(norm);
}
