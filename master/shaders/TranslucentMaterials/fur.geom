#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;

uniform vec4 light_pos;
uniform mat4 PVM;
uniform mat3 N;
uniform vec2 WIN_SCALE;           // precisely equal to: WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0
out vec3 color;
out vec3 tc;
uniform float s;
in vec3 _normal[];

void main(void)
{
    vec4 displace = vec4(s*_normal[0]*.1, 0);
    gl_Position = PVM*(gl_in[0].gl_Position+displace);
    tc = gl_in[0].gl_Position.xyz;
    color = vec3(.7,.5,.3)*dot(light_pos.xyz,N*_normal[0]);
    EmitVertex();

    displace = vec4(s*_normal[1]*.1, 0);
    gl_Position = PVM*(gl_in[1].gl_Position+displace);
    tc = gl_in[1].gl_Position.xyz;
    color = vec3(.7,.5,.3)*dot(light_pos.xyz,N*_normal[1]);
    EmitVertex();

    displace = vec4(s*_normal[2]*.1, 0);
    gl_Position = PVM*(gl_in[2].gl_Position+displace);
    tc = gl_in[2].gl_Position.xyz;
    color = vec3(.7,.5,.3)*dot(light_pos.xyz,N*_normal[2]);
    EmitVertex();

    EndPrimitive();
}
