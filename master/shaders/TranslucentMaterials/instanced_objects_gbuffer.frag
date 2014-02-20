#version 150

in vec3 p;
in vec3 n;
in vec4 m;

out vec4 fragData[3];

void main() 
{
        fragData[0].rgb = p;
        fragData[1].rgb = normalize(n);
        fragData[2] = m;
}
