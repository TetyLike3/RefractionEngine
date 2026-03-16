#pragma once

#include "BaseModel.h"

class BaseLight
{
public:
	BaseModel* m_pLightModel;
	float m_LightIntensity = 1.0f;
	glm::vec3 m_LightColor = glm::vec3(1.0f);

	BaseLight() {
		m_pLightModel = new BaseModel("./Rendering/Resources/models/Primitives/primitiveSphere.obj");
		m_pLightModel->m_pTransform->Translate(glm::vec3(10.0f, 5.0f, 10.0f));
		m_pLightModel->m_pTransform->Scale(glm::vec3(0.02f));
	}
private:
};

