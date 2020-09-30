#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

    //layout (location = 1) in vec3 color;

out vec2 texCoords;
out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    texCoords = texCoord;

    outColor = color;
}