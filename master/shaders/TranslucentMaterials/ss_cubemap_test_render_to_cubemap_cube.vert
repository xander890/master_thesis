#version 430
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

uniform mat4 PVM;
uniform mat4 M;
uniform vec3 center;

out vec3 _tex;
out vec3 _center;

void main()
{
    //_tex = vec3(M * vec4(vertex,1));
    _tex = mat3(M) * vertex;
    _center = vec3(M * vec4(vertex,1));
    gl_Position = PVM * vec4(vertex,1);
}
