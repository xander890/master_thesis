#version 150

uniform mat4 VM;
uniform mat4 M;
uniform mat4 PVM;
uniform mat3 N;

in vec3 vertex;
in vec3 normal;
in vec2 texcoord;
//in vec3 dpdu;
//in vec3 dpdv;

out vec3 _vertex;
out vec3 pos;
//out vec3 u_tangent;
//out vec3 v_tangent;
out vec3 norm;
out vec2 _texcoord;

void main()
{
        norm = normalize(N*normal);
        pos = (VM*vec4(vertex,1)).xyz;
//        u_tangent = normalize(N*dpdu);
//        v_tangent = normalize(N*dpdv);
        vec3 n = normalize(normal);
        _vertex = (M*vec4(vertex,1)).xyz;
        if(abs(n.z)>.9)
            _texcoord = vertex.xy;
        else if(abs(n.y)>0.9)
            _texcoord = vertex.zx;
        else if(abs(n.x)>0.9)
            _texcoord = vertex.yz;
        gl_Position = PVM*vec4(vertex,1);
	
}
