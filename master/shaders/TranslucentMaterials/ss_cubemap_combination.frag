#version 430

uniform samplerCube colorCubemap;
uniform samplerCube depthCubemap;

out vec4 fragColor;

in vec3 pos;

uniform float zNear;
uniform float zFar;
uniform float cameraSize;
const float bias = 0.2;

uniform vec3 centerWorldCoordinates;

float sample_cb_pos(in vec3 position, float compare)
{
    float s = texture(depthCubemap, position).r;
    float clipZ = 2.0f * s - 1.0f; //-1/1 range, no need to clip w because of ortho
    float z = ((clipZ + (zFar + zNear)/(zFar - zNear)) * (zFar - zNear))/-2;
    float z_world = cameraSize + z;

    if(z_world - bias < compare)
        return 1.0;
    else
        return 0.0;
}


float sample_cb_neg(in vec3 position, float compare)
{
    float s = texture(depthCubemap, position).r;
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
    fragColor = //vec4(pos,1.0)
            vec4(sample_cb_pos(vec3( cameraSize,pos.y, pos.z), pos.x)) + // +X
            vec4(sample_cb_neg(vec3(-cameraSize,pos.y, pos.z), pos.x)) + // -X
            vec4(sample_cb_pos(vec3(pos.x, cameraSize, pos.z), pos.y)) + // +Y
            vec4(sample_cb_neg(vec3(pos.x,-cameraSize, pos.z), pos.y)) + // -Y
            vec4(sample_cb_pos(vec3(pos.x,pos.y,  cameraSize), pos.z)) + // +Z
            vec4(sample_cb_neg(vec3(pos.x,pos.y, -cameraSize), pos.z))  // -Z


            //vec4(texture(depthCubemap, vec3(10.0f,pos.y,pos.z)).r) // +X
            //vec4(texture(depthCubemap, vec3(-10.0f,pos.y,pos.z)).r) // -X
            //vec4(texture(depthCubemap,  vec3(pos.x,10.0f,pos.z)).r) // +Y
            //vec4(texture(depthCubemap,  vec3(pos.x,-10.0f,pos.z)).r) // +Y


            ;
        fragColor = 1 - fragColor/6.0f;

/*
        fragColor = //vec4(pos,1.0)
                vec4(
                (sample_cb_pos(vec3( cameraSize,pos.y, pos.z), pos.x)) + // +X
                (sample_cb_neg(vec3(-cameraSize,pos.y, pos.z), pos.x)) , // -X
                (sample_cb_pos(vec3(pos.x, cameraSize, pos.z), pos.y)) + // +Y
                (sample_cb_neg(vec3(pos.x,-cameraSize, pos.z), pos.y)) , // -Y
                (sample_cb_pos(vec3(pos.x,pos.y,  cameraSize), pos.z)) + // +Z
                (sample_cb_neg(vec3(pos.x,pos.y, -cameraSize), pos.z))  // -Z
                ,2.0f)

                //vec4(texture(depthCubemap, vec3(10.0f,pos.y,pos.z)).r) // +X
                //vec4(texture(depthCubemap, vec3(-10.0f,pos.y,pos.z)).r) // -X
                //vec4(texture(depthCubemap,  vec3(pos.x,10.0f,pos.z)).r) // +Y
                //vec4(texture(depthCubemap,  vec3(pos.x,-10.0f,pos.z)).r) // +Y


                ;
        fragColor = (1 - clamp(fragColor,0,1));
        */
}
