// Filename: Triangle.vert
#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex { vec4 gl_Position; };
layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[]
(
    vec2(-1.0, 1.0), // Bottom left
    vec2(0.0, -1.0), // Center top
    vec2(1.0, 1.0)   // Bottom Right
);

vec3 colors[3] = vec3[]
(
    vec3(1.0, 0.0, 0.0), // Red
    vec3(0.0, 1.0, 0.0), // Green
    vec3(1.0, 1.0, 0.0)  // Yellow
);

void main() 
{
    // gl_VertexIndex contains the zero based vertex invocation index
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0f);
    fragColor = colors[gl_VertexIndex];
}