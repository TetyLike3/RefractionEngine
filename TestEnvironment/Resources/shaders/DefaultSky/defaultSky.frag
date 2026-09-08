#version 450

in vec2 TexCoords;

layout(std140, binding = 0) uniform DefaultUniformBlock {
    mat4 viewMat;
    mat4 projMat;
} ubo;

out vec4 FragColor;

vec3 Sky(in vec3 rd) {
    vec3 skyCol = vec3(0.2);
    skyCol = mix( skyCol, 0.2 * vec3(0.1), pow( 1.0 - max(rd.y, 0.0), 8.0 ) );
    
    // horizon
    skyCol = mix( skyCol, 0.6 * vec3(.2), pow( 1.0 - max(rd.y, 0.0), 16.0 ) );
    
    return skyCol;
}

void main() {
    vec3 viewFront = vec3(ubo.viewMat[0][2], ubo.viewMat[1][2], ubo.viewMat[2][2]);
    float pitch = asin(viewFront.y) + 0.5;
    vec3 col = Sky(normalize(vec3(TexCoords.x, TexCoords.y - pitch, 1.0)));
    
    FragColor = vec4(vec3(col),1.0);
}