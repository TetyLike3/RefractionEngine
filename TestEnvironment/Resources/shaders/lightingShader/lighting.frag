#version 450

const int NR_LIGHTS = 32;
struct Light {
	vec3 Position;
	vec3 Color;
	
	float Linear;
	float Quadratic;
	float Radius;
};

in VERT_OUT {
	vec2 TexCoords;
} VertOut;

uniform bool usingCFAA;
uniform int CFAAScale;
uniform sampler2D gDiffuse;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gSMR;
uniform sampler2D gDepth;
uniform sampler2D gCFAAData;

uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
uniform vec3 ambient;
uniform int dataView;

out vec4 FragColor;

float viewNear = 0.01;
float viewFar = 1000;

void main() {
	switch(dataView) {
	case 0: default: { // Final
		vec3 Diffuse;
		vec3 Normal;
		vec3 FragPos;
		float Specular;
		if(usingCFAA) { // Blend CFAA-upscaled texture samples
			vec2 texelSize = 1.0 / textureSize(gDepth, 0);
			int kernelSize = CFAAScale-1;
			
			Normal = texture(gNormal, VertOut.TexCoords).rgb;
			for(int x = -kernelSize; x <= kernelSize; x++) {
				if(x == 0) continue;
				for(int y = -kernelSize; y <= kernelSize; y++) {
					if(y == 0) continue;
					vec2 samplePos = VertOut.TexCoords + vec2(texelSize.x * x, texelSize.y * y);
					Diffuse *= texture(gDiffuse, samplePos).rgb;
					Diffuse /= 2;
					//Normal *= texture(gNormal, samplePos).rgb;
					//Normal /= 2;
					FragPos *= texture(gPosition, samplePos).rgb;
					FragPos /= 2;
					Specular *= texture(gSMR, samplePos).r;
					Specular /= 2;
				}
			}
		} else {
			Diffuse = texture(gDiffuse, VertOut.TexCoords).rgb;
			Normal = texture(gNormal, VertOut.TexCoords).rgb;
			FragPos = texture(gPosition, VertOut.TexCoords).rgb;
			Specular = texture(gSMR, VertOut.TexCoords).r;
		}
		if (Normal == vec3(0.0, 0.0, 0.0)) { discard; }

		// calculate lighting
		vec3 lighting = Diffuse * ambient;
		vec3 viewDir = normalize(viewPos - FragPos);
		for(int i = 0; i < NR_LIGHTS; ++i) {
			// calculate distance between light source and current fragment
			float fragDistance = length(lights[i].Position - FragPos);
			if(fragDistance < lights[i].Radius) {
				// diffuse
				vec3 lightDir = normalize(lights[i].Position - FragPos);
				vec3 diffuseVal = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;

				// specular
				vec3 halfwayDir = normalize(lightDir + viewDir);  
				float specPower = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
				vec3 specularVal = lights[i].Color * specPower * Specular;

				// attenuation
				float attenuation = 1.0 / (1.0 + lights[i].Linear * fragDistance + lights[i].Quadratic * fragDistance * fragDistance);
				diffuseVal *= attenuation;
				specularVal *= attenuation;
				lighting += diffuseVal + specularVal;
			}
		}
		FragColor = vec4(lighting, 1.0);
	} break;
	case 1: { // Depth
		float z = texture(gDepth, VertOut.TexCoords).r * 2.0 - 1.0;
		float linearDepth = (2.0 * viewNear * viewFar) / (viewFar + viewNear - z * (viewFar - viewNear));
		FragColor = vec4(vec3(linearDepth), 1.0);
	} break;
	case 2: { // Diffuse
		vec3 Diffuse = texture(gDiffuse, VertOut.TexCoords).rgb;
		FragColor = vec4(Diffuse.x, Diffuse.y, Diffuse.z, 1.0);
	} break;
	case 3: { // Specular
		float Specular = texture(gSMR, VertOut.TexCoords).r;
		FragColor = vec4(vec3(Specular), 1.0);
	} break;
	case 4: { // Normals
		vec3 Normal = texture(gNormal, VertOut.TexCoords).rgb;
		FragColor = vec4(Normal.x, Normal.y, Normal.z, 1.0);
	} break;
	case 5: { // CFAA Contrast
		float contrast = texture(gCFAAData, VertOut.TexCoords).r;
		FragColor = vec4(contrast, contrast, contrast, 1.0);
	} break; }
}