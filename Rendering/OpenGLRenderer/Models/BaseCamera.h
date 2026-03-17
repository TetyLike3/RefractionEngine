#pragma once

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

#include "Transform.h"
#include "Log.h"

class BaseCamera
{
public:
	Transform m_Transform;
	float FOVy = 70.0f;

	BaseCamera();

	void move(glm::vec3 dirInput, glm::vec3 angInput);
	void setCameraSpeed(float newSpeed) { m_cameraSpeed = newSpeed; };
	void setCameraSensitivity(float newSensitivity) { m_cameraSensitivity = newSensitivity; };

private:
	friend class Window;

	glm::vec3 m_cameraTarget;

	float m_yaw;
	float m_pitch;

	float m_cameraSpeed;
	float m_cameraSensitivity;
};