precision mediump float;

//uniform sampler2D tex;  // Uniform specifying the texture unit
uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;

varying vec3 _normal;
// in vec2 _texcoord;


void main()
{
    vec3 norm = normalize(_normal);

    vec4 base_color =  mat_diff; //texture(tex, _texcoord.xy)
    vec4 color = base_color*light_amb;

    // Compute dot product of normal and light source direction
    float d = dot(norm,normalize(light_pos.xyz));
    if(d>0.0)
    {
        color += base_color*d*light_diff;
        vec3 h = normalize(light_half_vec);
        float s = max(0.0, dot(norm, h));
        color += pow(s, mat_spec_exp) * light_spec*mat_spec;
    }
    gl_FragColor = color;
}
