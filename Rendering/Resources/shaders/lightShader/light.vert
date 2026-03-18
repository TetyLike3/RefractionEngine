#version 450

layout(std140, binding = 0) uniform DefaultUniformBlock {
	mat4 view;
	mat4 proj;
} ubo;

uniform mat4 modelTransform;

layout (location = 0) in vec3 aPos;

void main() {
	gl_Position = ubo.proj * ubo.view * modelTransform * vec4(aPos, 1);
}