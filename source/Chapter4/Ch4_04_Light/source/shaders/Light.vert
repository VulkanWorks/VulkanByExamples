// Filename: Light.vert
#version 450
#extension GL_ARB_separate_shader_objects : enable
layout (std140, binding = 0) uniform bufferVals {
mat4 normalMatrix; 	// Normal matrix
mat4 projection;   	// Projection matrix
mat4 view;			// View matrix
mat4 model;			// Model matrix
vec4 lightPosition;	// Light position
} UBO;

layout (location = 0) in vec4 inPosition; // Position attribute
layout (location = 1) in vec3 inNormal;   // Normals attribute
layout (location = 0) out vec4 fragColor; // Color attribute

out gl_PerVertex { 
    vec4 gl_Position;
};

vec3 LightColor    = vec3(0.6, 0.7, 0.4); // Light Color 
vec3 MaterialColor = vec3(0.3, 0.6, 0.5); // Material Color

void main() 
{
	vec4 pos = UBO.model * inPosition;
    vec3 nNormal = normalize(mat3(UBO.normalMatrix) * inNormal);
	vec3 eyeCoord = vec3 (UBO.view * inPosition);
    vec3 nLight = normalize(UBO.lightPosition.xyz - eyeCoord);

	float cosAngle = max( 0.0, dot( nNormal, nLight ));
	vec3 diffuse = MaterialColor * LightColor;
	fragColor = vec4(cosAngle * diffuse, 1);

    gl_Position = UBO.projection * UBO.view * UBO.model * inPosition;
}