#version 430

uniform sampler2D tex;  // Uniform specifying the texture unit

in vec4 _color;
out vec4 fragColor;

void main()
{
    fragColor = _color;
}



