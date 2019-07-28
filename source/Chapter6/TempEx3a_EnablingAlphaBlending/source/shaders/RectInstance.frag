// Filename: RectInstance.frag
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 fragColor;
layout(location = 0) out vec4 outColor;

void main() 
{
    // Pass through fragment color input as output
    outColor = fragColor;
	outColor.w = 0.5;
}