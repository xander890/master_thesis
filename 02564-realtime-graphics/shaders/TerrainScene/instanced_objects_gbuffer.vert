#version 150
#define SOLUTION_CODE
in vec3 vertex;
in vec3 normal;

out vec3 p;
out vec3 n;
out vec4 m;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;
#ifdef SOLUTION_CODE
uniform mat4 InstanceMatrix[100];
uniform vec4 mat_diff[100];
#endif

void main() 
{
#ifdef SOLUTION_CODE
    mat3 N2 = mat3(InstanceMatrix[gl_InstanceID]);
    gl_Position = PVM * (InstanceMatrix[gl_InstanceID]*vec4(vertex, 1));
    p = (VM * (InstanceMatrix[gl_InstanceID] * vec4(vertex , 1))).xyz;
    n = normalize(N*(N2*normal));
    m = mat_diff[gl_InstanceID];
#else
    gl_Position = PVM * vec4(vertex, 1);
    p = (VM * vec4(vertex, 1)).xyz;
    t = texcoord;
    n = N * normal;
#endif
}
