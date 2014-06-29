#version 430
smooth in vec3 pos;

uniform sampler2DRect skybox;

uniform vec2 cubemap_size;

layout(location = 0) out vec4 fragColor;

const float ONE_M_PI = 0.31830988618f;
uniform vec4 dir_lights[16];

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

    for(int i = 0; i < 16; i++)
    {
        vec3 dd = dir_lights[i].xyz;

       if(dot(normalize(pos),normalize(dd)) > 0.9999)
       {
            fragColor = vec4(1.0,0,0,0);
            //return;
       }
       else if (dot(normalize(pos),normalize(dir_lights[i].xyz)) < -0.9999)
       {
           //fragColor = vec4(0,1,0,0);
           //return;
       }
    }
    fragColor = texture(skybox,uv * cubemap_size);
//    fragColor = vec4(dir_lights[3]);
}
