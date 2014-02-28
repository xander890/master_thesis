#version 430
uniform sampler2D maintexture;

in vec3 _tex;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main(void)
{
    float val = texture(maintexture,_tex.xy).r;
    vec3 col = hsv2rgb(vec3(val,1.0,1.0));
    fragColor = vec4(val,0.0,0.0,1.0);
    //fragColor = vec4(_tex.xy,0.0,1.0);

}
