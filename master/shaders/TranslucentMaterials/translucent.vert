#version 150
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

out vec3 _normal;
out vec3 _texcoord;
out vec3 _light_pos;

uniform vec4 light_pos[50];

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;

void main()
{

    vec4 light = light_pos[0];
    _normal = normalize(N*normal);
    vec4 _position = VM * vec4(vertex,1);
    _light_pos = vec3(light - _position);

    _texcoord = texcoord;
    gl_Position = PVM * vec4(vertex,1);
}
