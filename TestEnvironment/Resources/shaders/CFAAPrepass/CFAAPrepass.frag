#version 450

layout (location = 0) out vec3 gDiffuse;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gPosition;
layout (location = 3) out vec3 gSMR;
layout (location = 4) out float gDepth;
layout (location = 5) out float gContrast; // For CFAA

in vec4 FragColor;
in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D tDiffuse;
uniform sampler2D tSpecular;

void main()
{    
    gDiffuse.rgb = texture(tDiffuse, TexCoords).rgb;
    gNormal = normalize(Normal);
    gPosition = FragPos;
    gSMR.r = texture(tSpecular, TexCoords).r;
}