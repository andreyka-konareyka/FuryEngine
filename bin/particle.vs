#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 aTexCoords;
// layout (location = 1) in vec3 normal;

out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    //FragPos = vec3(model * vec4(position, 1.0f));

    TexCoords = aTexCoords;
    TexCoords.y = 1.0f - TexCoords.y;
    //TexCoords = vec2(aTexCoords.x, 1.0f - aTexCoords.y);
} 