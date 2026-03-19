#include "BaseCamera.h"

BaseCamera::BaseCamera() {
	m_Transform = Transform();
	m_Transform.Translate(glm::vec3(0.0f, 0.0f, -5.0f));
	m_Transform.Rotate(glm::vec3(0.0f,glm::radians(180.0f),0.0f));
	m_cameraTarget = m_Transform.position + m_Transform.GetForwardVector();

	m_yaw = 0.0f;
	m_pitch = 0.0f;

	// Set default settings
	m_cameraSpeed = 1.0f;
	m_cameraSensitivity = 1.0f;
}

void BaseCamera::Move(glm::vec3 dirInput, glm::vec3 angInput) {
	// Normalize inputs
	if (dirInput != glm::vec3(0.0f)) dirInput = glm::normalize(dirInput);
	//if (angInput != glm::vec3(0.0f)) angInput = glm::normalize(angInput);


	// Move camera
	glm::vec3 translateDelta = glm::vec3();
	if (abs(dirInput.z) > 0.0f) translateDelta += (dirInput.z * m_cameraSpeed) * m_Transform.GetForwardVector();
	if (abs(dirInput.y) > 0.0f) translateDelta += m_Transform.GetUpVector() * (dirInput.y * m_cameraSpeed);
	if (abs(dirInput.x) > 0.0f) translateDelta += m_Transform.GetRightVector() * (dirInput.x * m_cameraSpeed);

	// Rotate camera
	m_yaw += angInput.y * m_cameraSensitivity;
	m_pitch = std::clamp(m_pitch + angInput.x * m_cameraSensitivity, -80.0f, 80.0f);

	glm::vec3 targetRotation = glm::vec3();
	targetRotation.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
	targetRotation.y = sin(glm::radians(m_pitch));
	targetRotation.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

	m_Transform.Translate(translateDelta);
	m_Transform.orientation = glm::quatLookAt(targetRotation, glm::vec3(0,1,0));
	//Log::Info("Moved camera by " + std::to_string(translateDelta.x) + ", " + std::to_string(translateDelta.y) + ", " + std::to_string(translateDelta.z));
	//Log::Info("Rotated camera by " + std::to_string(rotateDelta.x) + ", " + std::to_string(rotateDelta.y) + ", " + std::to_string(rotateDelta.z));

	m_cameraTarget = m_Transform.position + m_Transform.GetForwardVector();
}