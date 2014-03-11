#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit

in vec3 _normal;
in vec3 _texcoord;
in vec3 _light_pos;
in vec3 _transl;

uniform vec4 light_pos[50];
uniform vec4 light_diff[50];
uniform vec4 light_spec[50];
uniform vec4 light_amb;

uniform mat4 M;
uniform mat4 VM;
uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;


out vec4 fragColor;

void main()
{
    vec3 norm = normalize(_normal);
    vec3 light = normalize(_light_pos);

    float cos_theta = max(dot(norm, light), 0.0);

    // ambient and diffuse part
    vec4 color = (cos_theta*light_diff[0]);

    // specular part
    fragColor = color;

    fragColor = 30 * vec4(_transl,1.0);

}
