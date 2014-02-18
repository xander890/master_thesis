#version 150

uniform sampler2D potential_tex;
uniform sampler2D noise_tex;
uniform sampler1D particles_tex;

in float _texcoord;

const float dt = 0.0015; // Time step
const float h = 0.004; // Grid distance

float potential(vec2 tc, vec2 pos)
{
    float d = length(tc - pos);
    return texture(potential_tex, tc).w;
}

vec2 grad_potential(vec2 tc)
{
    float gx = (potential(tc + vec2(h, 0.0), tc)-potential(tc - vec2(h, 0.0), tc))/(2.0*h);
    float gy = (potential(tc + vec2(0.0, h), tc)-potential(tc - vec2(0.0, h), tc))/(2.0*h);
    return vec2(gx, gy);
}

out vec4 fragColor;
void main()
{
    vec4 pos_vel = texture(particles_tex, _texcoord);
    fragColor.xy = pos_vel.xy + dt * normalize(pos_vel.zw);
    fragColor.zw = normalize(pos_vel.zw + 0.05*texture(noise_tex, pos_vel.xy).xy + 0.3*texture(potential_tex, pos_vel.xy).xy - dt * 10.0 * grad_potential(pos_vel.xy));
}
