#version 430
in vec3 _tex;
uniform int colorMapSize;

restrict uniform layout(rgba32f, binding = 0) image2DArray colorMap;

out vec4 fragColor;

void main(void)
{
    ivec3 texadj = ivec3(_tex.xy * colorMapSize, gl_Layer);
    imageStore(colorMap, texadj, vec4(1.0f,0.5f,0.0f,0.0f));

    fragColor = vec4(_tex,1.0);
}
