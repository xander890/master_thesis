#version 150

uniform mat4 VM;
uniform mat4 PVM;

uniform float time;
uniform float delta_time;

in vec3 vertex;
in vec4 velocity;

out vec4 pos;
out vec4 pos_old;

#define SOLUTION_CODE
#ifdef SOLUTION_CODE
vec3 particle_position(vec3 p0, vec3 velocity, float t)
{
	return p0 + t * velocity + 0.5 * t * t * vec3(0.0,0.0,-9.82);
}

void main()
{
	float t0 = velocity.w;
	float t = mod(time-t0,0.4);
	float t_old = t - delta_time;

        pos = VM * vec4(particle_position(vertex, velocity.xyz, t), 1.0);
        pos_old = VM * vec4(particle_position(vertex, velocity.xyz, t_old), 1.0);
        gl_Position = PVM * vec4(particle_position(vertex, velocity.xyz, t), 1.0);
}
#else
void main()
{
        float t0 = velocity.w;
        float t = mod(time-t0,0.4);
        float t_old = t - delta_time;

        gl_Position = PVM * vec4(vertex, 1.0);
}
#endif
