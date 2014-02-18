#version 150
uniform mat4 VM;
uniform mat4 PVM;
uniform mat3 N;
uniform sampler1D particle_tex;

uniform float s;
uniform float time;


in vec3 vertex;
in vec3 normal;
in vec2 texcoord;

out vec3 pos;
out vec2 _texcoord;
out vec3 _norm; // A varying variable used to transfer the color from
// vertex to fragment program.

float sqr(float x){return x*x;}

void main()
{
#define SOLUTION_CODE
#ifdef SOLUTION_CODE
    vec4 p = vec4(vertex, 1);
    p.y += p.x>0.0?sin(2.0*time)*sqr(sin(p.x)):0.0;

    vec4 pos_vel = texture(particle_tex, s);
    vec2 vel2d = normalize(pos_vel.zw);
    vec2 pos2d = 9.0*pos_vel.xy - 4.5;

    vec4 pos_o;
    pos_o.x = -vel2d.x * p.x + vel2d.y * p.y + pos2d.x;
    pos_o.y = -vel2d.y * p.x - vel2d.x * p.y + pos2d.y;
    pos_o.zw = p.zw;

    vec3 norm_o;
    norm_o.x = -vel2d.x * normal.x + vel2d.y * normal.y;
    norm_o.y = -vel2d.y * normal.x - vel2d.x * normal.y;
    norm_o.z = normal.z;

    // Compute eyespace normal and position
    pos = (VM * pos_o).xyz;
    _norm = normalize(N*norm_o);
    _texcoord = texcoord;
    gl_Position = PVM * pos_o;
#else
    // Compute eyespace normal and position
    _norm = normalize(N*normal);
    _texcoord = texcoord;
    gl_Position = PVM * vec4(vertex,1);
#endif
}
