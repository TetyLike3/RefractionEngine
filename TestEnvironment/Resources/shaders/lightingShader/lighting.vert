#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

layout(std140, binding = 0) uniform DefaultUniformBlock {
	mat4 viewMat;
	mat4 projMat;
} ubo;

uniform bool usingCFAA;
uniform int dataView;

out VERT_OUT {
	vec2 TexCoords;
} VertOut;

void main() {
	if (usingCFAA && dataView != 5) { // Sample GBuffer textures at native resolution
		VertOut.TexCoords = aTexCoords / 2;
	} else {
		VertOut.TexCoords = aTexCoords;
	}
	gl_Position = vec4(aPos, 1.0);
}