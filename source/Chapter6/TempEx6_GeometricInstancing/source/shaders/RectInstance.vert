// Filename: RectInstance.vert
#version 450
#extension GL_ARB_separate_shader_objects : enable
layout (std140, binding = 0) uniform TransformBufferStruct {
	mat4 mvp;
} TransformBuffer;

// Instance attributes
layout (location = 2, binding = 0) in mat4 instancePos;
layout(location = 0) out vec4 fragColor;

// Vextex attributes
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;


out gl_PerVertex { 
    vec4 gl_Position;
};

void main() 
{
    fragColor = inColor;
	gl_Position   = TransformBuffer.mvp * instancePos * (inPosition);
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;
}