#version 150

noperspective in vec3 dist;
const vec4 WIRE_COL = vec4(1.0,0.0,0.0,1);
const vec4 FILL_COL = vec4(1,1,1,1);
out vec4 fragColor;
void main(void)
{
    // Compute the distance to the closest edge by taking
    // the minimum.
    float d = min(dist[0],min(dist[1],dist[2]));

    // Map minimum distance to edge intensity
    float I = exp2(-2*d*d);

    // Use edge intensity to interpolate between wire and fill color.
    fragColor = I*WIRE_COL + (1.0 - I)*FILL_COL;
}
