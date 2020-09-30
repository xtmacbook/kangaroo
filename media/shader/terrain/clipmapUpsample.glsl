--VERTEX-base
#version 330 core
layout(location = 0) in vec2 position;

out vec2 fsTextureCoordinates;

uniform mat4 viewportOrthographicMatrix;
uniform vec2 sourceOrigin;
uniform vec2 updateSize;
uniform vec2 destinationOffset;
uniform vec2 oneOverTextureSize;
uniform mat4 model;

void main()                     
{
    vec2 scaledPosition = position * updateSize;
    gl_Position = viewportOrthographicMatrix * vec4(scaledPosition + destinationOffset, 0.0, 1.0);
    fsTextureCoordinates = (scaledPosition * 0.5 + sourceOrigin) * oneOverTextureSize;
}
--FRAGMENT-base
#version 330 core
in vec2 fsTextureCoordinates;
out vec4 texelOutput;
uniform sampler2D texture0; // coarse map
void main()
{
    texelOutput = texture(texture0, fsTextureCoordinates);
}
