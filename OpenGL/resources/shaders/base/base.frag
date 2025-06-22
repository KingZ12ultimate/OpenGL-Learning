#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
uniform vec3 color;
uniform bool white;

void main()
{
    FragColor = white ? vec4(color, 1.0) : texture(texture1, TexCoords);
}
