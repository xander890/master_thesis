#version 150

uniform mat4 V;

uniform sampler2D tex;  // Uniform specifying the texture unit
//uniform sampler2D tex_bump;  // Uniform specifying the texture unit
uniform vec4 light_pos;
uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;
uniform float is_reflected;

in vec3 _vertex;
in vec3 pos;
//in vec3 u_tangent;
//in vec3 v_tangent;
in vec3 norm;
in vec2 _texcoord;

const int MAX_STEP = 200;
const float MAX_HEIGHT = 0.005;
const int IMAGE_DIMS = 1024;

out vec4 fragColor;


vec4 phongLighing(vec3 norm_mapped, vec2 uv){
    vec3 ray_dir = normalize(pos);
    vec3 lpos = (V*vec4(0,0,4,1)).xyz;
    vec3 light_direction = (lpos-pos);
    float d = (dot(norm_mapped, normalize(light_direction)))/(0.3*length(light_direction));
    vec4 base_color = mat_diff * texture(tex, _texcoord);
    vec4 color = 0.3*base_color;
    if(d>0.0)
    {
        color += d*base_color;
            vec3 h = normalize(-ray_dir+light_direction);
            float s = dot(norm_mapped, h);
            if(s>0.0)
                    color += d*pow(s, mat_spec_exp)*mat_spec;
    }
    return color;
}

void main()
{
    if( _vertex.z > 0.5 || is_reflected<0.5)
        fragColor = phongLighing(norm, pos.xy);
    else
        discard;
}
