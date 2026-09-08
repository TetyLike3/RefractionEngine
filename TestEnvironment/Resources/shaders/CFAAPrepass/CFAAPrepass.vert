#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout(std140, binding = 0) uniform DefaultUniformBlock {
    mat4 viewMat;
    mat4 projMat;
} ubo;

uniform mat4 modelTransform;

out vec4 FragColor;
out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
    vec4 worldPos = modelTransform * vec4(aPos, 1.0);
    FragPos = worldPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(modelTransform)));
    Normal = normalMatrix * aNormal;

    gl_Position = ubo.projMat * ubo.viewMat * worldPos;
}