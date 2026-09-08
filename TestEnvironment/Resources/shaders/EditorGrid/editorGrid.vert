#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec2 aUV;

layout(std140, binding = 0) uniform DefaultUniformBlock {
    mat4 viewMat;
    mat4 projMat;
} ubo;

out vec3 FragPos;
out vec2 UV;

void main()
{
    float gridDiv = 10.0;

    vec4 worldPos = vec4(aPos, 1.0);
    vec3 cameraPos = inverse(ubo.viewMat)[3].xyz;
    vec3 cameraCenteringOffset = floor(cameraPos / gridDiv) * gridDiv;
    FragPos.yx = (worldPos.xyz - cameraCenteringOffset).yx;
    FragPos.z = worldPos.z;
    UV = aUV;

    gl_Position = ubo.projMat * ubo.viewMat * worldPos;
}
