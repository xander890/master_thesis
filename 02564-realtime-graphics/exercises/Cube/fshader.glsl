#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#endif

uniform float light_intensity;

varying vec3 v_normal;

//! [0]
void main()
{
    // Set fragment color from texture
    gl_FragColor = light_intensity*vec4(.7,.9,.6,1.0)*dot(v_normal, vec3(0,0,1));
}
//! [0]

