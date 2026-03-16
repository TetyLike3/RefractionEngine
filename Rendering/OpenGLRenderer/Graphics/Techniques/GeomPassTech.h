#pragma once
#include "BaseTechnique.h"

class GeomPassTech : public BaseTechnique {
public:
	GeomPassTech();

	virtual bool Init();

	void SetWVP(const glm::mat4& WVP);
	void SetWorldMatrix(const glm::mat4& WVP);
	void SetColorTexUnit(unsigned int TexUnit);

private:
	GLuint m_WVPLocation = 0;
	GLuint m_WorldMatrixLocation = 0;
	GLuint m_ColorTexUnitLocation = 0;
};