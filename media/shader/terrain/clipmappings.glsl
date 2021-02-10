--VERTEX-base
#version 330 core
layout(location = 0) in vec3 position;

out vec2 fsTextureCoordinates;

uniform mat4 viewportOrthographicMatrix;
uniform vec2 sourceOrigin;
uniform vec2 updateSize;
uniform vec2 destinationOffset;

void main()
{
    vec2 scaledPosition = position.xy * updateSize;
    gl_Position  = viewportOrthographicMatrix * vec4(scaledPosition + destinationOffset, -1.0, 1.0);
    fsTextureCoordinates = scaledPosition + sourceOrigin;
}

--FRAGMENT-base
#version 330 core
in vec2 fsTextureCoordinates;

layout (location = 0) out mediump float texelOutput;
uniform sampler2DRect texture0;
void main()
{
    texelOutput = texture(texture0,fsTextureCoordinates).r;
}
