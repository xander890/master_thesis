#version 150
#define SOLUTION_CODE
in vec3 _normal;
in vec4 _mat_diff;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;


out vec4 fragColor;

void main()
{
#ifdef SOLUTION_CODE
        vec3 norm = normalize(_normal);
	// Compute dot product of normal and light source direction
        float d = dot(norm,normalize(light_pos.xyz));

        vec4 base_color = _mat_diff;
	vec4 color = base_color*light_amb;
	if(d>0.0)
		color += base_color*d*light_diff;
        fragColor = color;
#else
    fragColor = vec4(1.0);
#endif
}
