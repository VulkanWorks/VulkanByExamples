// Filename: Triangle.vert
#version 450
#extension GL_ARB_separate_shader_objects : enable
layout (std140, binding = 0) uniform bufferVals {
mat4 mvp;
} myBufferVals;

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec4 inColor;
// Instanced attributes
layout (location = 2) in mat4 instancePos;
layout (location = 6) in vec4 instanceRot;
layout (location = 7) in vec4 instanceCol;
layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 texCoord;

out gl_PerVertex { 
    vec4 gl_Position;
};

void main() 
{
    //gl_Position = myBufferVals.mvp * inPosition;
    vec4 inPositionNew = inPosition;
//	fragColor = inColor;
	fragColor = instanceCol;
	texCoord = inPosition;
	inPositionNew.x = inPosition.x * instanceRot.x;
	inPositionNew.y = inPosition.y * instanceRot.y;
    gl_Position   = myBufferVals.mvp * instancePos * (inPositionNew /*+ instanceRot*/);
	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;

}