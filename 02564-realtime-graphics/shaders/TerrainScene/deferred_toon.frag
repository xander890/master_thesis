#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex; 
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

float sqr(float x) {return x*x;}

out vec4 fragColor;
#ifdef SOLUTION_CODE
float is_gedge()
{
	const float edge_thresh = 0.4;
	float e = 
                texture(gtex, gl_FragCoord.xy+vec2(1,0)).z
                +texture(gtex, gl_FragCoord.xy+vec2(-1,0)).z
                +texture(gtex, gl_FragCoord.xy+vec2(0,1)).z
                +texture(gtex, gl_FragCoord.xy+vec2(0,-1)).z
                - 4.0*texture(gtex, gl_FragCoord.xy).z;
	return max(0.0,1.0-abs(e));
}

float is_nedge()
{
//	const float edge_thresh = 0.95;
//	vec3 n = texture(ntex, gl_FragCoord.xy).xyz;
//	vec3 nu = texture(ntex, gl_FragCoord.xy+vec2(0,1)).xyz;
//	vec3 nd = texture(ntex, gl_FragCoord.xy+vec2(0,-1)).xyz;
//	vec3 nl = texture(ntex, gl_FragCoord.xy+vec2(-1,0)).xyz;
//	vec3 nr = texture(ntex, gl_FragCoord.xy+vec2(1,0)).xyz;
//	float e = dot(n,nu)*dot(n,nd)*dot(n,nl)*dot(n,nr);
//	
//	return smoothstep(0.98  , 0.99, e);
	const float edge_thresh = 0.94;
	vec3 e = 
                texture(ntex, gl_FragCoord.xy+vec2(1,0)).xyz
                +texture(ntex, gl_FragCoord.xy+vec2(-1,0)).xyz
                +texture(ntex, gl_FragCoord.xy+vec2(0,1)).xyz
                +texture(ntex, gl_FragCoord.xy+vec2(0,-1)).xyz
                - 4.0*texture(ntex, gl_FragCoord.xy).xyz;
	return smoothstep(0.2  , 0.1, max(e.x,max(e.y,e.z)));

}


void main()
{
        float z = texture(gtex, gl_FragCoord.xy).z;
        vec4 base_color = texture(ctex, gl_FragCoord.xy);
        if(z == -1000.0)
                fragColor  = vec4(0.4,0.3,1,0);
        else
        {
                vec3 light_dir = light_pos.xyz;
                vec3 n = texture(ntex, gl_FragCoord.xy).xyz;
                vec4 color = dot(n,light_dir) > 0.0 ? base_color : 0.5 * base_color;
                vec4 highlight = dot(n, light_half_vec) > 0.99 ? vec4(base_color.w) : vec4(0.0);
                fragColor = is_gedge() * is_nedge()* (color + highlight);
        }
}
#else
void main()
{
        float z = texture(gtex, gl_FragCoord.xy).z;
        vec4 base_color = texture(ctex, gl_FragCoord.xy);
        if(z == -1000.0)
                fragColor  = vec4(0.4,0.3,1,0);
        else
        {
            fragColor = base_color;
        }
}
#endif
