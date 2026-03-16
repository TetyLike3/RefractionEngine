#pragma once

#include <GLAD/glad.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "../../Utilities.h"
#include "Log.h"

class BaseBuffer {
public:
protected:
	GLuint m_ID;
};