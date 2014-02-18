#version 150
#define SOLUTION_CODE
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 P;

in vec4 pos_old[1];
in vec4 pos[1];

void main(void)
{
#ifdef SOLUTION_CODE
        vec3 dir = normalize(pos[0].xyz-pos_old[0].xyz);
        vec3 cross_dir = 0.003*cross(dir, -normalize(pos[0].xyz));

	vec4 p_o_1 = pos_old[0];
        p_o_1.xyz += cross_dir;

	vec4 p_o_2 = pos_old[0];
        p_o_2.xyz -= cross_dir;

	vec4 p_1 = pos[0];
        p_1.xyz += cross_dir;

	vec4 p_2 = pos[0];
        p_2.xyz -= cross_dir;

	gl_Position = P * p_2;
        EmitVertex();
        gl_Position = P * p_1;
        EmitVertex();
        gl_Position = P * p_o_2;
        EmitVertex();
        gl_Position = P * p_o_1;
        EmitVertex();
#endif
}
