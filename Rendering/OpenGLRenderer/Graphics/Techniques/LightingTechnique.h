#pragma once
#include "BaseTechnique.h"
#include "../../Models/DirectionalLight.h"

class LightingTechnique : public BaseTechnique {
public:
	LightingTechnique();

	virtual bool Init();

	void SetWVP(const glm::mat4& WVP);
	void SetTextureUnit(unsigned int TextureUnit);
	void SetDirectionalLight(const DirectionalLight& Light);

private:
	GLuint m_WVPLocation = 0;
	GLuint m_SamplerLocation = 0;
	GLuint m_DirLightColorLocation = 0;
	GLuint m_DirLightAmbientIntensityLocation = 0;
};