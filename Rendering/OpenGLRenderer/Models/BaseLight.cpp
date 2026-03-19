#include "BaseLight.h"

BaseLight::BaseLight() {
	mLightShader = ShaderManager::GetShaderByName("lightingShader");
	mTransform = new Transform();
}

void BaseLight::UpdateShaderUniforms(unsigned int index) {
	const float linear = 1.0f;
	const float quadratic = 1.2f;
	const float radius = 80.0f;
	mLightShader->setUniformVec3("lights[" + std::to_string(index) + "].Position", mTransform->position);
	mLightShader->setUniformVec3("lights[" + std::to_string(index) + "].Color", mLightColor);
	mLightShader->setUniformFloat("lights[" + std::to_string(index) + "].Linear", linear);
	mLightShader->setUniformFloat("lights[" + std::to_string(index) + "].Quadratic", quadratic);
	mLightShader->setUniformFloat("lights[" + std::to_string(index) + "].Radius", radius);
}