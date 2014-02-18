#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform float light_intensity;
uniform sampler2D texture;

varying vec3 v_normal;
varying vec2 v_tex;

#define SOLUTION_CODE 1

void main()
{
#ifdef SOLUTION_CODE
    // Set fragment color from texture
    gl_FragColor = light_intensity*vec4(.7,.9,.6,1.0)*dot(v_normal, vec3(0,0,1));
    gl_FragColor = light_intensity*texture2D(texture,v_tex)*dot(v_normal, vec3(0,0,1));
#else
    gl_FragColor = texture2D(texture,v_tex);
#endif
}

