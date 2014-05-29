#version 430
smooth in vec3 pos;

uniform sampler2DRect skybox;

uniform vec2 cubemap_size;

layout(location = 0) out vec4 fragColor;

const float ONE_M_PI = 0.31830988618f;

vec2 vector_cubemap_to_uv(vec3 pos)
{
    vec3 p = normalize(pos);
    p.z = -p.z;
    p = p.yzx;
    return vec2(0.5f + 0.5f * atan(p.x,-p.z) * ONE_M_PI, acos(p.y) * ONE_M_PI);
}

void main()
{

    vec2 uv = vector_cubemap_to_uv(pos);
    fragColor = texture(skybox,uv * cubemap_size);
   // fragColor = vec4(pos,1.0)
}
