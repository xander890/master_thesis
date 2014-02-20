#version 150
#define SOLUTION_CODE
in vec3 vertex;
in vec3 normal;

out vec3 _normal;
out vec4 _mat_diff;

uniform mat4 PVM;
uniform mat3 N;
#ifdef SOLUTION_CODE
uniform mat4 InstanceMatrix[100];
uniform vec4 mat_diff[100];
#endif

void main()
{
#ifdef SOLUTION_CODE
    mat3 N2 = mat3(InstanceMatrix[gl_InstanceID]);
    _normal = normalize(N*(N2*normal));
    _mat_diff = mat_diff[gl_InstanceID];
    vec4 v = InstanceMatrix[gl_InstanceID]*vec4(vertex,1);
    gl_Position = PVM * v;
#else
    gl_Position = PVM * v;
#endif
}
