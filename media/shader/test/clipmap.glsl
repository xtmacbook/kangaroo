--VERTEX-Compiled
#version 330 core

#define MAX_ANISOTROPY 16
#define MIP_LEVELS_MAX 7

uniform mat4 view;
uniform mat4 projection; 


uniform vec3 g_EyePosition;
uniform vec3 g_LightPosition;
uniform vec3 g_WorldRight;
uniform vec3 g_WorldUp;


uniform vec2 g_StackCenter;
uniform ivec2 g_TextureSize;
uniform uint g_StackDepth;
uniform vec2 g_ScaleFactor;
uniform int g_SphereMeridianSlices;
uniform int g_SphereParallelSlices;
uniform float g_ScreenAspectRatio;
uniform vec3 g_MipColors[MIP_LEVELS_MAX];

void main()
{
    
    float meridianPart = ( gl_VertexID % ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereMeridianSlices );
    float parallelPart = ( gl_VertexID / ( g_SphereMeridianSlices + 1 ) ) / float( g_SphereParallelSlices );
    
    float angle1 = meridianPart * 3.14159265 * 2.0;
    float angle2 = ( 0.5 - parallelPart ) * 3.14159265;
    
    float cos_angle1 = cos( angle1 );
    float sin_angle1 = sin( angle1 );
    float cos_angle2 = cos( angle2 );
    float sin_angle2 = sin( angle2 );
        
    vec3 VertexPosition;
  
    VertexPosition.x = cos_angle1 * cos_angle2;
    VertexPosition.y = sin_angle2;
    VertexPosition.z = sin_angle1 * cos_angle2;
    
    gl_Position = projection * view * vec4(VertexPosition,1.0);

}

--FRAGMENT-trilinear
#version 330 

uniform sampler2D PyramidTexture;
uniform sampler2D PyramidTextureHM;
uniform sampler2DArray StackTexture;

uniform ivec2 g_TextureSize;

out vec4 color;

void main()
{
    color = texture(StackTexture,vec3(0.5,0.5,2.0));
}
