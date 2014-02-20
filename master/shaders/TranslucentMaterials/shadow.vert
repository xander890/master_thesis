#version 150
#define SOLUTION_CODE
in vec3 vertex;
uniform mat4 PVM;
#ifdef SOLUTION_CODE
uniform mat4 InstanceMatrix[100];

void main()
{
    gl_Position = PVM * InstanceMatrix[gl_InstanceID] * vec4(vertex, 1);
}
#else
void main()
{
    gl_Position = PVM * vec4(vertex, 1);
}

#endif
