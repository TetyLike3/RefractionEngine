#pragma once

#include "Transform.h"
#include "BaseCamera.h"


struct PerspectiveProperties {
	float FOV = 0.0f;
	float Width = 0.0f;
	float Height = 0.0f;
	float zNear = 0.0f;
	float zFar = 0.0f;
};

struct OrthographicProperties {
	float Right = 0.0f;
	float Left = 0.0f;
	float Bottom = 0.0f;
	float Top = 0.0f;
	float zNear = 0.0f;
	float zFar = 0.0f;

	float Width = 0.0f;
	float Height = 0.0f;
};


class Pipeline {
public:
	Pipeline() {
		m_Scale = glm::vec3(1.0f);
		m_WorldPos = glm::vec3(1.0f);
		m_Rotation = glm::vec3(1.0f);
	}

	void Scale(const glm::vec3& scale) {
		Scale(scale.x, scale.y, scale.z);
	}
	void Scale(float x, float y, float z) {
		m_Scale.x = x;
		m_Scale.y = y;
		m_Scale.z = z;
	}

	void WorldPos(float x, float y, float z) {
		m_WorldPos.x = x;
		m_WorldPos.y = y;
		m_WorldPos.z = z;
	}
	void WorldPos(const glm::vec3 pos) {
		WorldPos(pos.x, pos.y, pos.z);
	}

	void Rotate(float x, float y, float z) {
		m_Rotation.x = x;
		m_Rotation.y = y;
		m_Rotation.z = z;
	}
	void Rotate(const glm::vec3 rotation) {
		Rotate(rotation.x, rotation.y, rotation.z);
	}

	void SetPerspectiveProperties(const PerspectiveProperties& p) {
		m_PersProps = p;
	}

	void SetOrthographicProperties(const OrthographicProperties& p) {
		m_OrthoProps = p;
	}

	void SetCamera(BaseCamera* pCamera) {
		m_pCamera = pCamera;
	}

	const glm::mat4 GetWPTransform();
	const glm::mat4 GetWVTransform();
	const glm::mat4 GetVPTransform();
	const glm::mat4 GetWVPTransform();
	const glm::mat4 GetWVOrthoPTransform();
	const glm::mat4 GetWorldTransform();
	const glm::mat4 GetViewTransform();
	const glm::mat4 GetProjTransform();
	
private:
	glm::vec3 m_Scale;
	glm::vec3 m_WorldPos;
	glm::vec3 m_Rotation;

	PerspectiveProperties m_PersProps;
	OrthographicProperties m_OrthoProps;

	BaseCamera* m_pCamera;
	struct {
		glm::vec3 Pos;
		glm::vec3 Target;
		glm::vec3 Up;
	} m_Camera;

	glm::mat4 m_WVPTransform;
	glm::mat4 m_VPTransform;
	glm::mat4 m_WPTransform;
	glm::mat4 m_WVTransform;
	glm::mat4 m_WTransform;
	glm::mat4 m_VTransform;
	glm::mat4 m_PTransform;
};

