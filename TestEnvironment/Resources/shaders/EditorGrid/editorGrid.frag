#version 450

layout (location = 0) out vec3 gPosition;

in vec3 FragPos;
in vec2 UV;

out vec4 FragColor;

float lineWidth = 1;

void main()
{    
    float lineAA = fwidth(UV.x);
    float lineUV = abs(UV.x * 2.0);
    float line = smoothstep(lineWidth + lineAA, lineWidth - lineAA, lineUV);

    gPosition = FragPos;
    FragColor = vec4(line, line, line, 1.0);
}