#version 330 core

out vec4 color;

in vec3 FragPos;
in vec2 TexCoords;

uniform vec4 objectColor;
uniform sampler2D texture1;

void main()
{
    color = texture(texture1, TexCoords);
} 