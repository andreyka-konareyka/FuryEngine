//fragment shader
#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D fboAttachment;
uniform float logoAlpha;

void main()
{
	vec4 color = texture(fboAttachment, TexCoords);
    FragColor = vec4(color.rgb, color.a * logoAlpha);
}