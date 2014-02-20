#version 150

uniform sampler2D noise_tex;

#define SOLUTION_CODE

in vec3 p;
in vec3 pos;
in vec3 n;
in vec3 t;

const vec4 col_low = vec4(0.2,0.5,0.4,1.0);
const vec4 col_high = vec4(0.8,0.75,0.5,1.0);

out vec4 fragData[3];
void main() 
{

        fragData[0].rgb = p;
        fragData[1].rgb = normalize(n);
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
        base_color.w = 0.0;
        fragData[2] = base_color;
;
}
