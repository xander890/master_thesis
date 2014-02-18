#version 150
#define SOLUTION_CODE

uniform sampler2D noise_tex;
uniform sampler1D particles_tex;
uniform int n_particles;
uniform vec2 user_pos;

in float _texcoord;
const float dt = 0.0016; // Time step
const float radius_user = 0.06;  // The radius of the user used for repelling from user
const float radius_repel = 0.025; // Radius of Gaussian blob used for repelling
#ifdef SOLUTION_CODE
const float radius_flock = 0.025; // Radius of Gaussian blob used for flocking
const float force_repel = 0.002; // The scaling used for the vector that pushes away from the walls, other fish, user.
const float force_flock = 0.3;   // Scaling used when adding the velocities of other fish.
#endif

float Gaussian(vec2 v,float r2)
{
    return exp(-dot(v,v)/r2);
}

#ifdef SOLUTION_CODE
float sqr(float x) {return x*x;}

vec2 gradient_of_Gaussian(vec2 v,float r2)
{
    float d = dot(v,v);
    return d>0 ? -2*v/(sqrt(d)*r2)*exp(-d/r2) : vec2(0);
}
#endif

out vec4 fragColor;
void main()
{

#ifdef SOLUTION_CODE
    vec4 fish_data = texture(particles_tex, _texcoord);
    vec2 pos = fish_data.xy;
    vec2 vel = fish_data.zw;
    vec2 new_vel = vel;

    new_vel += 0.05*texture(noise_tex, pos).xy;
    for (int i = 0; i < n_particles; ++i)
    {
        float s = (float(i)+0.5)/float(n_particles);
        vec4 fish = texture(particles_tex, s);
        // also add other fish's velocity fields to image.
        new_vel += force_flock * Gaussian(pos-fish.xy,sqr(radius_flock)) * fish.zw;
        new_vel -= force_repel * gradient_of_Gaussian(pos-fish.xy, sqr(radius_repel));
    }
    new_vel -= force_repel * gradient_of_Gaussian(pos-user_pos,sqr(radius_repel));
    new_vel -= force_repel * gradient_of_Gaussian(vec2(pos.x, 0),sqr(radius_repel));
    new_vel -= force_repel * gradient_of_Gaussian(vec2(pos.x-1, 0),sqr(radius_repel));
    new_vel -= force_repel * gradient_of_Gaussian(vec2(0,pos.y),sqr(radius_repel));
    new_vel -= force_repel * gradient_of_Gaussian(vec2(0,pos.y-1),sqr(radius_repel));

    new_vel = normalize(new_vel);
    vec2 new_pos = pos + dt * new_vel;
    fragColor = vec4(new_pos, new_vel);
#else
    fragColor = vec4(0.5,0.5,1,0);
#endif
}
