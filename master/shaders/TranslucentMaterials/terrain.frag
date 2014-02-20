#version 150
//#define SOLUTION_CODE
uniform vec4 light_pos;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

in vec3 pos;
in vec3 norm;

const vec4 col_low = vec4(0.2,0.5,0.4,1.0);
const vec4 col_high = vec4(0.8,0.75,0.5,1.0);

uniform sampler2D noise_tex;

out vec4 fragColor;

void main()
{
	// Compute very simple diffuse shading intensity
	float diff = dot(norm, normalize(light_pos.xyz));
	
	// Map height to color
        float h = 0.1 * pos.z + 0.05 * texture(noise_tex, 0.1*pos.xy).r;
#ifdef SOLUTION_CODE
        vec4 base_color =  ((1.0-h)*col_low + h * col_high) * (0.5+
                            0.25*texture(noise_tex,1.00*pos.xy).r+
                            0.25*texture(noise_tex,0.5*pos.xy).r+
                            0.25*texture(noise_tex,0.25*pos.xy).r);
#else
        vec4 base_color =  col_low;
#endif
        fragColor = base_color * ( light_diff*diff + light_amb);
}
