// Filename: Triangle.frag
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 texCoord;

layout(location = 0) out vec4 outColor;

void main() 
{
    // Pass through fragment color input as output
    outColor = texCoord;
	///////////////////////////

	float weight = 0.0f;
    float dx     = texCoord.x - 0.5;
    float dy     = texCoord.y - 0.5;
    float length = sqrt(dx * dx + dy * dy);
    
    // Calculate the weights
    weight = smoothstep( 0.4, 0.45, length );

    outColor = mix( vec4(fragColor.rgb, 1.0), vec4(1.0, 0.0, 0.0, 0.0), weight);
    outColor = fragColor;
}