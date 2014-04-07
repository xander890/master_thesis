#version 430

//#define COLORS

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

out vec4 fragColor;

in vec3 pos;

uniform float zNear;
uniform float zFar;
uniform float cameraSize;
const float bias = 0.2;

uniform vec3 centerWorldCoordinates;

float sample_cb_pos(samplerCube cb, in vec3 position, float compare)
{
    float s = texture(cb, position).r;
    float clipZ = 2.0f * s - 1.0f; //-1/1 range, no need to clip w because of ortho
    float z = ((clipZ + (zFar + zNear)/(zFar - zNear)) * (zFar - zNear))/-2;
    float z_world = cameraSize + z;

    if(z_world - bias < compare)
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
    if(z_world + bias < compare)
        return 0.0;
    else
        return 1.0;
}

void main(void)
{
    vec3 plusx =  vec3( cameraSize,pos.y, pos.z);
    float visplusx = sample_cb_pos(depthCubemap,plusx, pos.x);
    vec3 minusx = vec3(-cameraSize,pos.y, pos.z);
    float visminusx = sample_cb_neg(depthCubemap,minusx, pos.x);
    vec3 plusy =  vec3(pos.x, cameraSize, pos.z);
    float visplusy = sample_cb_pos(depthCubemap,plusy, pos.y);
    vec3 minusy = vec3(pos.x,-cameraSize, pos.z);
    float visminusy = sample_cb_neg(depthCubemap,minusy, pos.y);
    vec3 plusz =  vec3(pos.x,pos.y,  cameraSize);
    float visplusz = sample_cb_pos(depthCubemap,plusz, pos.z);
    vec3 minusz = vec3(pos.x,pos.y, -cameraSize);
    float visminusz = sample_cb_neg(depthCubemap,minusz, pos.z);

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
       // fragColor = vec4(visplusx + visminusx + visplusy + visminusy + visplusz + visminusz) / 6;

#else




        fragColor =
                    visplusx  * texture(colorCubemap, plusx).rgba +
                    visminusx * texture(colorCubemap, minusx).rgba +
                    visplusy  * texture(colorCubemap, plusy).rgba +
                    visminusy * texture(colorCubemap, minusy).rgba +
                    visplusz  * texture(colorCubemap, plusz).rgba +
                    visminusz * texture(colorCubemap, minusz).rgba
                    ;

        fragColor /= (visplusx + visminusx + visplusy + visminusy + visplusz + visminusz) ;
#endif
}
