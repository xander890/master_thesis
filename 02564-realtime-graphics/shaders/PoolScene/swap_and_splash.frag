#version 150

uniform sampler2D tex;
uniform int add_splash;
uniform vec2 splash_center;

in vec2 tc;

float splash_radius = 0.004;

out vec4 fragColor;
void main()
{
    fragColor = texture(tex, tc);
    if (add_splash != 0)
    {
        vec2 v;
        v.x = tc.x - splash_center.x;
        v.y = tc.y - splash_center.y;
        float xx = (v.x*v.x + v.y*v.y)/(splash_radius*splash_radius);
        fragColor.x -=0.15* exp(-xx);
    }
}
