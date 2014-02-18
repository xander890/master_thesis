#version 150
#define SOLUTION_CODE

uniform sampler2D tex;  // Uniform specifying the texture unit
uniform sampler2D tex_bump;  // Uniform specifying the texture unit
uniform vec4 light_pos;

in vec3 pos;
in vec3 u_tangent;
in vec3 v_tangent;
in vec3 norm;
in vec2 _texcoord;

const int MAX_STEP = 200;
const float MAX_HEIGHT = 0.0075;
const int IMAGE_DIMS = 1024;

out vec4 fragColor;

#ifdef SOLUTION_CODE
vec3 find_normal(vec2 tc)
{
    float udiff = 3.0*MAX_HEIGHT*(texture(tex_bump, tc+vec2(0.001,0)).r-texture(tex_bump, tc-vec2(0.001,0)).r)/0.002;
    float vdiff = 3.0*MAX_HEIGHT*(texture(tex_bump, tc+vec2(0,0.001)).r-texture(tex_bump, tc-vec2(0,0.001)).r)/0.002;
    return normalize(norm-udiff*u_tangent-vdiff*v_tangent);
    //return vec3(udiff, vdiff,0);
}


vec2 parallaxMapping(vec2 uv){
    vec3 ray_dir = normalize(pos);
    vec3 ray_dir_ts = vec3(dot(u_tangent,ray_dir),dot(v_tangent,ray_dir),dot(norm, ray_dir));
    ray_dir_ts *= 0.25/(float(IMAGE_DIMS)*max(0.00001,length(ray_dir_ts.xy)));
    vec3 tc = vec3(uv, MAX_HEIGHT);
    int i=0;
    float h;
    for(;i<MAX_STEP;++i)
    {
            tc += ray_dir_ts;
            h = MAX_HEIGHT*(texture(tex_bump, tc.xy).r);
            if(tc.z<h)
                    break;
    }
    return tc.xy;
}

vec4 phongLighing(vec2 uv, vec3 norm_mapped){
    vec3 ray_dir = normalize(pos);
    vec3 light_direction = normalize(light_pos.xyz-pos);
    float d = dot(norm_mapped, light_direction);
    vec4 color = vec4(0.3);
    if(d>0.0)
    {
            color += d*vec4(0.6);
            vec3 h = normalize(-ray_dir+light_direction);
            float s = dot(norm_mapped, h);
            if(s>0.0)
                    color += d*pow(s, 40.0)*vec4(0.3);
    }
    return color * texture(tex, uv.xy);
}
#endif

void main()
{
#ifdef SOLUTION_CODE
        vec2 tc = parallaxMapping(_texcoord);

        vec3 norm_mapped = find_normal(tc);

        fragColor = phongLighing(tc, norm_mapped);
#else
    vec3 light_direction = normalize(light_pos.xyz-pos);
    float d = dot(norm, light_direction);
    vec4 color = vec4(0.3)+d*vec4(0.6);
    fragColor = color * texture(tex, _texcoord.xy);
#endif
}
