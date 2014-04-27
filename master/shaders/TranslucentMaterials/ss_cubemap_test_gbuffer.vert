#version 430
in vec3 vertex;

out vec3 _tex;

void main()
{
    _tex = vec3(vertex.x * 0.5 + 0.5,vertex.y * 0.5 + 0.5,0.0f);
    gl_Position = vec4(vertex.xy,0,1);
}