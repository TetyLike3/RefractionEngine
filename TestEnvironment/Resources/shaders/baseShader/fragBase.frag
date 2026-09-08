#version 450

layout(std140, binding = 0) uniform DefaultUniformBlock {
	mat4 viewMat;
	vec3 viewPos;
	mat4 projMat;
} ubo;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuseSpecular;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texDiffuse1;
uniform sampler2D texSpecular1;


void main() {
	gPosition = FragPos;
	gNormal = normalize(Normal);
	gDiffuseSpecular.a = texture(texSpecular1, TexCoord).a;

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(ubo.viewPos - FragPos);
	
	//gDiffuseSpecular.rgb = texture(texDiffuse1, TexCoord).rgb;
	gDiffuseSpecular.rgb = vec3(1,1,1);
}