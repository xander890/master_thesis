attribute vec3 vertex;

uniform mat4 PVM;

void main( void )
{
    gl_Position = PVM*vec4(vertex, 1.0);
}
