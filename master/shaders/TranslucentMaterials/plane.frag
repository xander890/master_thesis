#version 430
uniform sampler2D maintexture;
uniform sampler2DShadow shadow;
uniform mat4 Mat;

in vec3 _tex;
in vec3 pos;

out vec4 fragColor;



float sample_shadow_map(vec3 pos)
{
    vec4 light_pos = Mat * vec4(pos,1.0f);
    light_pos.z -= 0.004;
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    //return light_pos.z - 1;
    return texture(shadow,light_pos.xyz).r;

/*    float visibility = 1.0f;
    if(sh < light_pos.z)
    {
        visibility = 0.0f;
    }
    //return 1-light_pos.z;
    return visibility;
    */
}

void main(void)
{

    float val = sample_shadow_map(pos);
    fragColor = vec4(1.0,0.0,0.0,1.0) *(val * 0.5 + 0.5);

}
