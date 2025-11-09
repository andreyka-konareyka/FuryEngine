#version 330 core

out vec4 color;

in vec3 FragPos;
in vec2 TexCoords;

uniform vec4 objectColor;
uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
	
	if (texColor.a < 0.01)
	{
	    discard;
	}
	
    color = texColor;
} 