#version 150
#define SOLUTION_CODE
in vec2 _texcoord;
uniform sampler2D hf_tex;

float damping = 0.998;
float time_step = 0.0125;
float ds = 0.0009765;
float c = 0.02;

float u(vec2 tc) {return texture(hf_tex, tc).x;}

out vec4 fragColor;
void main() 
{
#ifdef SOLUTION_CODE
    vec2 old_uv = texture(hf_tex, _texcoord).xy;
    float v_new = old_uv.y + c * c *(u(vec2(_texcoord.x + ds, _texcoord.y)) +
                                     u(vec2(_texcoord.x - ds, _texcoord.y)) +
                                     u(vec2(_texcoord.x, _texcoord.y + ds)) +
                                     u(vec2(_texcoord.x, _texcoord.y - ds)) - 4.0 * u(_texcoord)) * time_step / (ds * ds);
    //	v_new *= damping;
    float u_new = old_uv.x * damping + v_new * time_step;
    fragColor.x = u_new;
    fragColor.y = v_new;
#else
    fragColor = vec4(0.0);
#endif

}
