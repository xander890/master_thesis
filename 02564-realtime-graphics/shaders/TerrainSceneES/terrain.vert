
attribute vec3 vertex;
attribute vec3 normal;

uniform mat4 PVM;
uniform mat3 N;

varying vec3 pos;
varying vec3 norm;

void main()
{
        pos = vertex;
        norm = normalize(N * normal);
        gl_Position = PVM * vec4(vertex, 1);
}
