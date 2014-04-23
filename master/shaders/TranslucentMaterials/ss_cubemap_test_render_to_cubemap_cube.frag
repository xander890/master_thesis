#version 430
in vec3 _tex;
in vec3 _center;

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;
uniform float areacircle;

uniform float one_over_max_samples;
uniform float total_area;

out vec4 fragColor;

void main(void)
{
    vec4 smp = texture(colorCubemap, vec3(_tex.x,_tex.y,_tex.z));
    fragColor = one_over_max_samples * total_area * smp;
    //fragColor = vec4(smp.xyz,1.0);
}
