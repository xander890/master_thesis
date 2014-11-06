#version 430
#define RANDOM
#define TIME
#define MULTI_LIGHTS
//#define OVERLAY
#define SPECTRAL
#define BLENDING

layout(location = 0) out vec4 fragColor;

#include "ss_aincludes_constants.glinc"

uniform sampler2DArray ntex;
uniform sampler2DArray vtex;
uniform sampler1D discpoints;

#ifdef TIME
uniform sampler2DArray colorMap;
#endif

smooth in vec3 position;
smooth in vec3 norm;

uniform vec4 light_pos[MAX_LIGHTS];
uniform vec4 light_diff[MAX_LIGHTS];
uniform int light_number;
uniform mat4 lightMatrices[MAX_LIGHTS];


uniform float one_over_max_samples;
uniform float one_over_discs;
uniform float min_tr;
uniform float max_samples;
#ifdef TIME
uniform int convergence_frames;
uniform float current_frame_percentage;
uniform mat4 cameraMatrices[DIRECTIONS];
uniform int global_frame;
uniform int current_frame;
#endif

uniform float discradius;
uniform int samples;

#include "ss_aincludes_ss_uniforms.glinc"

#include "ss_aincludes_optics.glinc"

#include "ss_aincludes_jensen_bssrdf.glinc"

#include "ss_aincludes_random.glinc"


void main(void)
{
   return;
}
