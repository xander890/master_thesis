#version 150
uniform sampler1D particles_tex;

uniform int n_particles;
uniform vec2 user_pos;

in vec2 _texcoord;

const float radius = 0.04; // Radius of Gaussian blob used for repelling
const float flocking_radius = 0.055; // Radius of Gaussian blob used for flocking

float Gaussian(vec2 v,float r2)
{
    return exp(-dot(v,v)/r2);
}

out vec4 fragColor;
void main()
{
    vec4 col = vec4(0.0);
    float r2 = radius * radius;
    float fr2 = flocking_radius*flocking_radius;
    
    // Repell from other fish
    for (int i = 0; i < n_particles; ++i)
    {
        float s = (float(i)+0.5)/float(n_particles);
        vec2 p_i = texture(particles_tex, s).xy;
        float d2 = Gaussian(_texcoord-p_i,r2);
        
        col.w += d2;
        // also add other fish's velocity fields to image.
        col.rg += Gaussian(_texcoord-p_i,fr2) * texture(particles_tex, s).zw;
    }
    
    // Repell from walls
    col.w += 2.0*Gaussian(vec2(_texcoord.x,0.0),r2);
    col.w += 2.0*Gaussian(vec2(0.0,_texcoord.y),r2);
    vec2 tc = vec2(1.0, 1.0) - _texcoord;
    col.w += 2.0*Gaussian(vec2(tc.x,0.0),r2);
    col.w += 2.0*Gaussian(vec2(0.0,tc.y),r2);

    // Repell from user
    col.w += Gaussian(user_pos-_texcoord,r2);
    
    fragColor = col;
}
