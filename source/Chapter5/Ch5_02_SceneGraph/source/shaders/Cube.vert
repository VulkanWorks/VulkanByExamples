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
layout(location = 0) out vec4 fragColor;

out gl_PerVertex { 
    vec4 gl_Position;
};

void main() 
{
    //gl_Position = myBufferVals.mvp * inPosition;
    fragColor = inColor;
    gl_Position   = myBufferVals.mvp * instancePos * (inPosition /*+ instanceRot*/);

	gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0;

}