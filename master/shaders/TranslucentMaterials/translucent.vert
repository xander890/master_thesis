#version 150
in vec3 vertex;
in vec3 normal;
in vec3 translucent;
in vec2 texcoord;

out vec3 _normal;
out vec3 _texcoord;
out vec3 _light_pos;
out vec3 _transl;

uniform vec4 light_pos[50];

uniform mat4 PVM;
uniform mat4 VM;
uniform mat4 M;
uniform mat3 N;

void main()
{

    vec4 light = light_pos[0];
    _normal = (vertex * mat3(M));
    vec4 _position = VM * vec4(vertex,1);
    _light_pos = vec3(light - _position);
    _transl = vec3(translucent);

    gl_Position = PVM * vec4(vertex,1);
}
