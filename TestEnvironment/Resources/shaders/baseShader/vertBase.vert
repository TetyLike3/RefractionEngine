#version 450

layout(std140, binding = 0) uniform DefaultUniformBlock {
	mat4 viewMat;
	vec3 viewPos;
	mat4 projMat;
} ubo;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 modelTransform;

void main() {
	vec4 worldPos = modelTransform * vec4(aPos, 1.0);
	FragPos = worldPos.xyz;
	TexCoord = aTexCoord;
	Normal = aNormal;

	gl_Position = ubo.projMat * ubo.viewMat * worldPos;
}