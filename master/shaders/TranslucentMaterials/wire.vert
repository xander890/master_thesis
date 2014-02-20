#version 150
in vec3 vertex;
uniform mat4 PVM;
#define SOLUTION_CODE

#ifdef SOLUTION_CODE
uniform mat4 InstanceMatrix[100];
#endif




void main()
{
#ifdef SOLUTION_CODE
    gl_Position = PVM * InstanceMatrix[gl_InstanceID] * vec4(vertex, 1);
#else
    gl_Position = PVM * vec4(vertex.xyz, 1);
#endif
}
