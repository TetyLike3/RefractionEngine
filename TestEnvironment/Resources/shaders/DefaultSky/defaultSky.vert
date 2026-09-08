#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout(std140, binding = 0) uniform DefaultUniformBlock {
    mat4 viewMat;
    mat4 projMat;
} ubo;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}