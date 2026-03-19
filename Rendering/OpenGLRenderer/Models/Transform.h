#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

class Transform {
public:
	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;

	Transform();

	void Translate(glm::vec3 delta);
	void Rotate(float angle, glm::vec3 axis);
	void Rotate(glm::vec3 delta);
	void Rotate(glm::quat delta);
	void Scale(glm::vec3 delta);

	glm::mat4 GetTransform() const;
	glm::vec3 GetForwardVector() const { return glm::rotate(orientation, glm::vec3(0.0f,0.0f,-1.0f)); };
	glm::vec3 GetRightVector() const { return glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f)); };
	glm::vec3 GetUpVector() const { return glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f)); };

	// Utility
	static bool AreQuaternionsSimilar(glm::quat quatA, glm::quat quatB) {
		return (abs(dot(quatA, quatB) - 1.0) < 0.001);
	}
	static glm::quat LerpQuaternion(glm::quat quatA, glm::quat quatB, float alpha) {
		return mix(quatA, quatB, alpha);
	}
	static glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 end) {
		start = normalize(start);
		end = normalize(end);

		float cosTheta = dot(start, end);
		glm::vec3 rotationAxis;

		// Case for vectors facing opposite directions
		if (cosTheta < -1 + 0.001f) {
			rotationAxis = glm::cross(glm::vec3(0.0f,0.0f,1.0f), start);

			if (glm::length2(rotationAxis) < 0.01) {
				rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);
			}

			rotationAxis = normalize(rotationAxis);
			return glm::angleAxis(glm::radians(180.0f), rotationAxis);
		}

		rotationAxis = cross(start, end);

		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;

		return glm::quat(
			s * 0.5f,
			rotationAxis.x * invs,
			rotationAxis.y * invs,
			rotationAxis.z * invs
		);
	}
	static glm::quat LookAt(glm::vec3 eye, glm::vec3 target, glm::vec3 targetUp) {
		glm::vec3 direction = target - eye;

		glm::quat rot1 = RotationBetweenVectors(glm::vec3(0.0f, 0.0f, 1.0f), direction);

		glm::vec3 right = glm::cross(direction, targetUp);
		targetUp = glm::cross(right, direction);

		glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);
		glm::quat rot2 = RotationBetweenVectors(newUp, targetUp);

		return rot2 * rot1;
	}
};