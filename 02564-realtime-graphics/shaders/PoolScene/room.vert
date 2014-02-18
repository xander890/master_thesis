#version 150

uniform mat4 VM;
uniform mat4 PVM;
uniform mat3 N;

in vec3 vertex;
in vec3 normal;
in vec2 texcoord;
in vec3 dpdu;
in vec3 dpdv;

out vec3 pos;
out vec3 u_tangent;
out vec3 v_tangent;
out vec3 norm;
out vec2 _texcoord;

void main()
{
        norm = normalize(N*normal);
        pos = (VM*vec4(vertex,1)).xyz;
        u_tangent = normalize(N*dpdu);
        v_tangent = normalize(N*dpdv);
        _texcoord = texcoord;
        gl_Position = PVM*vec4(vertex,1);
	
}
