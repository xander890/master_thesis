#version 150

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
uniform sampler2D tex;

in vec3 p;
in vec3 n;
in vec3 t;

out vec4 fragData[3];

void main() 
{
        fragData[0].rgb = p;
        fragData[1].rgb = normalize(n);
        fragData[2] = mat_diff*texture(tex, t.xy);
        fragData[2].w = length(mat_spec.xyz);
}
