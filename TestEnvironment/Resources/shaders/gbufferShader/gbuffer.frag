#version 450

layout (location = 0) out vec3 gDiffuse;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gPosition;
layout (location = 3) out vec3 gSMR;
layout (location = 4) out float gDepth;
layout (location = 5) out vec3 gCFAAData;

in VERT_OUT {
	vec4 FragColor;
	vec3 FragPos;
	vec2 TexCoords;
	vec2 ScreenTexCoords;
	vec3 Normal;
} VertOut;

uniform bool usingCFAA;
uniform int CFAAScale;

// Model textures
uniform sampler2D tDiffuse;
uniform sampler2D tSpecular;

// CFAA prepass data
uniform sampler2D tCFAADiffuse;
uniform sampler2D tCFAANormal;
uniform sampler2D tCFAAPosition;
uniform sampler2D tCFAASMR;
uniform sampler2D tCFAADepth;

void main() {
	if(usingCFAA) { // Use CFAA prepass data, expect to sample to upscaled image
		// Get depth contrast
		vec2 texelSize = 1.0 / textureSize(tCFAADepth, 0);
		vec2 originalTexCoords = VertOut.ScreenTexCoords / CFAAScale;
		float depthContrast;
		float pixelDepth = texture(tCFAADepth, originalTexCoords).r;

		int kernelSize = CFAAScale-1;
		for(int x = -kernelSize; x <= kernelSize; x++) {
			if(x == 0) continue;
			for(int y = -kernelSize; y <= kernelSize; y++) {
				if(y == 0) continue;
				depthContrast += (texture(tCFAADepth, VertOut.ScreenTexCoords + vec2(texelSize.x * x, texelSize.y * y)).r - pixelDepth);
			}
		}
		//contrast -= (1 - texture(tCFAADepth, originalTexCoords).rgb); // Offset by actual sample depth to get average difference
		gCFAAData.x = depthContrast;

		if (gCFAAData.x < 0.5) {
			// Skip extra sampling and just output the pre-sampled values
			gDiffuse = texture(tCFAADiffuse, originalTexCoords).rgb;
			gNormal = texture(tCFAANormal, originalTexCoords).rgb;
			gPosition = texture(tCFAAPosition, originalTexCoords).rgb;
			gSMR = texture(tCFAASMR, originalTexCoords).rgb;
			gDepth = texture(tCFAADepth, originalTexCoords).r;
		} else {
			// Further sample the object
			gDiffuse.rgb = texture(tDiffuse, VertOut.TexCoords).rgb;
			gNormal = normalize(VertOut.Normal);
			gPosition = VertOut.FragPos;
			gSMR.r = texture(tSpecular, VertOut.TexCoords).r;
		}
	} else {
		gDiffuse.rgb = texture(tDiffuse, VertOut.TexCoords).rgb;
		gNormal = normalize(VertOut.Normal);
		gPosition = VertOut.FragPos;
		gSMR.r = texture(tSpecular, VertOut.TexCoords).r;
	}
}