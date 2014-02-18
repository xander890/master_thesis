
attribute vec3 vertex;
attribute vec3 normal;

varying vec3 _normal;

uniform mat4 PVM;
uniform mat3 N;

void main()
{
    gl_Position = PVM * vec4(vertex, 1.0);
    _normal = normalize(N*normal);
    //	_texcoord = texcoord;
}
