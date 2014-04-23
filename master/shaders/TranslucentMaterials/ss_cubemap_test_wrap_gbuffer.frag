#version 430
uniform sampler2D maintexture;
uniform sampler2D vtex;
uniform sampler2D ntex;

uniform mat4 lightMatrix;

in vec3 pos;

out vec4 fragColor;

float sample_shadow_map(vec3 pos)
{
    vec4 light_pos = lightMatrix * vec4(pos,1.0f);
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    if(texture(vtex,light_pos.xy).z < light_pos.z)
    {
        return 1.0f;
    }
    else
    {
        return 0.0f;
    }
}

void main(void)
{
    float val = sample_shadow_map(pos);
    vec4 light_pos = lightMatrix * vec4(pos,1.0f);
    fragColor = texture(ntex,light_pos.xy);
//    fragColor = vec4(val,0.0,0.0,1.0);
}
