#version 430

//#define COLORS
//#define XONLY
uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

uniform vec4 light_pos[50];

out vec4 fragColor;

in vec3 position;
in vec3 norm;

// camera parameters (in order to reconstruct depth information)
uniform float zNear;
uniform float zFar;
uniform float cameraSize;

// epsilons (error control)
uniform float shadow_bias;
uniform float epsilon_combination;

uniform float one_over_max_samples;
uniform float total_area;
uniform float disc_area;


float sample_cb_pos(samplerCube cb, in vec3 position, float compare)
{
    float s = texture(cb, position).r;
    float clipZ = 2.0f * s - 1.0f; //-1/1 range, no need to clip w because of ortho
    float z = ((clipZ + (zFar + zNear)/(zFar - zNear)) * (zFar - zNear))/-2;
    float z_world = cameraSize + z;

    if(z_world - shadow_bias < compare)
        return 1.0;
    else
        return 0.0;
}


float sample_cb_neg(samplerCube cb, in vec3 position, float compare)
{
    float s = texture(cb, position).r;
    float clipZ = 2.0f * s - 1.0f; //-1/1 range, no need to clip w because of ortho
    float z = ((clipZ + (zFar + zNear)/(zFar - zNear)) * (zFar - zNear))/-2;
    float z_world = -(cameraSize + z);
    //return s;
    if(z_world + shadow_bias < compare)
        return 0.0;
    else
        return 1.0;
}

void main(void)
{
    float epsilon = 0.01;
    vec3 no = normalize(norm);
    vec3 wi = vec3(light_pos[0]);
    wi = normalize(wi);
    //vec3 offset = epsilon * (no - wi * dot(no,wi));
    vec3 offset = epsilon_combination * no;
    vec3 pos = position - offset;
    vec3 plusx =  vec3( cameraSize,pos.y, pos.z);
    float visplusx = sample_cb_pos(depthCubemap,plusx, pos.x);
    vec4 colorplusx = texture(colorCubemap, plusx);

    vec3 minusx = vec3(-cameraSize,pos.y, pos.z);
    float visminusx = sample_cb_neg(depthCubemap,minusx, pos.x);
    vec4 colorminusx = texture(colorCubemap, minusx);

    vec3 plusy =  vec3(pos.x, cameraSize, pos.z);
    float visplusy = sample_cb_pos(depthCubemap,plusy, pos.y);
    vec4 colorplusy = texture(colorCubemap, plusy);

    vec3 minusy = vec3(pos.x,-cameraSize, pos.z);
    float visminusy = sample_cb_neg(depthCubemap,minusy, pos.y);
    vec4 colorminusy = texture(colorCubemap, minusy);

    vec3 plusz =  vec3(pos.x,pos.y,  cameraSize);
    float visplusz = sample_cb_pos(depthCubemap,plusz, pos.z);
    vec4 colorplusz = texture(colorCubemap, plusz);

    vec3 minusz = vec3(pos.x,pos.y, -cameraSize);
    float visminusz = sample_cb_neg(depthCubemap,minusz, pos.z);
    vec4 colorminusz = texture(colorCubemap, minusz);

#ifdef COLORS
    fragColor = //vec4(pos,1.0)
            vec4(sample_cb_pos(depthCubemap,vec3( cameraSize,pos.y, pos.z), pos.x)) + // +X
            vec4(sample_cb_neg(depthCubemap,vec3(-cameraSize,pos.y, pos.z), pos.x)) + // -X
            vec4(sample_cb_pos(depthCubemap,vec3(pos.x, cameraSize, pos.z), pos.y)) + // +Y
            vec4(sample_cb_neg(depthCubemap,vec3(pos.x,-cameraSize, pos.z), pos.y)) + // -Y
            vec4(sample_cb_pos(depthCubemap,vec3(pos.x,pos.y,  cameraSize), pos.z)) + // +Z
            vec4(sample_cb_neg(depthCubemap,vec3(pos.x,pos.y, -cameraSize), pos.z))  // -Z


            //vec4(texture(depthCubemap, vec3(10.0f,pos.y,pos.z)).r) // +X
            //vec4(texture(depthCubemap, vec3(-10.0f,pos.y,pos.z)).r) // -X
            //vec4(texture(depthCubemap,  vec3(pos.x,10.0f,pos.z)).r) // +Y
            //vec4(texture(depthCubemap,  vec3(pos.x,-10.0f,pos.z)).r) // +Y


            ;
        fragColor = 1 - fragColor/6.0f;


        fragColor = //vec4(pos,1.0)
                vec4(
                (sample_cb_pos(depthCubemap,vec3( cameraSize,pos.y, pos.z), pos.x)) + // +X
                (sample_cb_neg(depthCubemap,vec3(-cameraSize,pos.y, pos.z), pos.x)) , // -X
                (sample_cb_pos(depthCubemap,vec3(pos.x, cameraSize, pos.z), pos.y)) + // +Y
                (sample_cb_neg(depthCubemap,vec3(pos.x,-cameraSize, pos.z), pos.y)) , // -Y
                (sample_cb_pos(depthCubemap,vec3(pos.x,pos.y,  cameraSize), pos.z)) + // +Z
                (sample_cb_neg(depthCubemap,vec3(pos.x,pos.y, -cameraSize), pos.z))  // -Z
                ,2.0f)

                //vec4(texture(depthCubemap, vec3(10.0f,pos.y,pos.z)).r) // +X
                //vec4(texture(depthCubemap, vec3(-10.0f,pos.y,pos.z)).r) // -X
                //vec4(texture(depthCubemap,  vec3(pos.x,10.0f,pos.z)).r) // +Y
                //vec4(texture(depthCubemap,  vec3(pos.x,-10.0f,pos.z)).r) // +Y


                ;
        fragColor = (fragColor / 2);
        fragColor = vec4(max(max(visplusz + visminusz, visplusx + visminusx),visplusy + visminusy)) / 2;
       // fragColor = vec4(visplusx + visminusx + visplusy + visminusy + visplusz + visminusz) / 6;

#else


#ifdef XONLY
    vec3 color = visminusy * colorminusy.rgb;

    fragColor = vec4(color,1.0f);

    //fragColor = fragColor * total_area * one_over_max_samples;

#else
        vec3 color =
                    visplusx  * colorplusx.rgb
                +   visminusx * colorminusx.rgb
                +   visplusy  * colorplusy.rgb
                +   visminusy * colorminusy.rgb
                +   visplusz  * colorplusz.rgb
                +   visminusz * colorminusz.rgb
                   ;

        fragColor = vec4(color,1.0f);
        fragColor.rgb /= visplusx + visminusx + visplusy + visminusy+ visplusz + visminusz;
        fragColor = fragColor * disc_area * one_over_max_samples;

 #endif
 //       fragColor = vec4(10 * offset, 1.0);
//        fragColor = vec4(colorplusx.a / 10);
#endif
}
