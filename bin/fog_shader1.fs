#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

vec2 newTexCoords;

vec4 fColor;
vec4 Fogcolor = vec4(0.4, 0.4, 0.4, 1.0);
float density = 0.01;
const float LOG2 = 1.442695;
float fogFactor;
float z;

void main()
{
    //newTexCoords = vec2(TexCoords.x, 1.0f - TexCoords.y);
    //FragColor = texture(texture_diffuse1, newTexCoords);

    newTexCoords = vec2(TexCoords.x, 1.0f - TexCoords.y);
    fColor = texture2D(texture_diffuse1, newTexCoords);
    z = gl_FragCoord.z / gl_FragCoord.w;
	fogFactor = exp2( -density * density * z * z * LOG2 );
	fogFactor = clamp(fogFactor, 0.0, 1.0);
        
        gl_FragColor = mix(Fogcolor, fColor, fogFactor );
}