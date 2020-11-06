--VERTEX-Compiled
#version 330 core

layout (location = 0) in vec3 vertex; 

uniform int g_SphereMeridianSlices;
uniform int g_SphereParallelSlices;

uniform mat4 view;
uniform mat4 projection; 


uniform vec3 g_EyePosition;
uniform vec3 g_LightPosition;

out VertexData
{
	vec4 position;
	vec2 texCoord;
	vec3 viewVectorTangent;
	vec3 lightVectorTangent;
}outData;

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

    outData.position = gl_Position;
    outData.texCoord = vec2( 1.0 - meridianPart, 1.0 - parallelPart );
    
    vec3 tangent = vec3( cos_angle1, 0.0, -sin_angle1 );
    vec3 binormal = vec3( -sin_angle1 * sin_angle2, cos_angle2, -cos_angle1 * sin_angle2 );
        
    vec3 viewVector = normalize(g_EyePosition - VertexPosition);
    
    outData.viewVectorTangent.x = dot( viewVector, tangent );
    outData.viewVectorTangent.y = dot( viewVector, binormal);
    outData.viewVectorTangent.z = dot( viewVector, VertexPosition );
    
    vec3 lightVector = normalize( g_LightPosition );
    
    outData.lightVectorTangent.x = dot( lightVector, tangent );

     
}



--FRAGMENT-base
#version 330 core

out vec4 color;

void main()
{
    color = vec4(0.0,1.0,0.0, 1.0f);
}
