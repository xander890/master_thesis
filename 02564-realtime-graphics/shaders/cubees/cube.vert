attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texcoord;

uniform mat4 PVM;
uniform mat3 N;

varying vec3 v_normal;
varying vec2 v_tex;

void main()
{
    // Calculate vertex position in screen space
    gl_Position = PVM * vec4(vertex, 1.0);

    // Pass texture coordinate to fragment shader
    v_tex = texcoord;

    // Value will be automatically interpolated to fragments inside polygon faces
    v_normal = N * normal;
}
