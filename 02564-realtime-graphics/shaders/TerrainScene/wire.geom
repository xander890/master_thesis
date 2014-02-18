#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

#define SOLUTION_CODE

uniform vec2 WIN_SCALE;           // precisely equal to: WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0
noperspective out vec3 dist;  // Vec3 containing the distances from a vertex to opposite edge

void main(void)
{
#ifdef SOLUTION_CODE
        vec2 p0 = WIN_SCALE * gl_in[0].gl_Position.xy/gl_in[0].gl_Position.w;
        vec2 p1 = WIN_SCALE * gl_in[1].gl_Position.xy/gl_in[1].gl_Position.w;
        vec2 p2 = WIN_SCALE * gl_in[2].gl_Position.xy/gl_in[2].gl_Position.w;
	
	vec2 v0 = p2-p1;
	vec2 v1 = p2-p0;
	vec2 v2 = p1-p0;
	float area = abs(v1.x*v2.y - v1.y * v2.x);
	
	dist = vec3(area/length(v0),0,0);
        gl_Position = gl_in[0].gl_Position;
	EmitVertex();
		
	dist = vec3(0,area/length(v1),0);
        gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	
	dist = vec3(0,0,area/length(v2));
        gl_Position = gl_in[2].gl_Position;
	EmitVertex();
#else
    dist = vec3(50,0,0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dist = vec3(0,50,0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dist = vec3(0,0,50);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
#endif
}
