#version 430
uniform sampler2D maintexture;
uniform sampler2D shadow;
uniform mat4 Mat;

in vec3 _tex;
in vec3 pos;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


float sample_shadow_map(vec3 pos)
{
    vec4 light_pos = Mat * vec4(pos,1.0f);
    light_pos.z -= 0.0015;
    if(light_pos.x < 0.0 || light_pos.x > 1.0) return 1.0;
    if(light_pos.y < 0.0 || light_pos.y > 1.0) return 1.0;
    //return light_pos.z - 1;
    return texture(shadow,light_pos.xy);

}

void main(void)
{

    float val = sample_shadow_map(pos);
    fragColor = vec4(0.5);


}
