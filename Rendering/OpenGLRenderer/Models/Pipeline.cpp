#include "Pipeline.h"

const glm::mat4 Pipeline::GetWPTransform() {
	auto wT = GetWorldTransform();
	auto pT = GetProjTransform();
	return pT * wT;
}

const glm::mat4 Pipeline::GetWVTransform() {
	auto wT = GetWorldTransform();
	auto vT = GetViewTransform();
	return vT * wT;
}

const glm::mat4 Pipeline::GetVPTransform() {
	auto vT = GetViewTransform();
	auto pT = GetProjTransform();
	return pT * vT;
}

const glm::mat4 Pipeline::GetWVPTransform() {
	auto wT = GetWorldTransform();
	auto vT = GetViewTransform();
	auto pT = GetProjTransform();
	return pT * vT * wT;
}

const glm::mat4 Pipeline::GetWVOrthoPTransform() {
	return glm::mat4(); // Not Implemented
}

const glm::mat4 Pipeline::GetWorldTransform() {
	Transform wT = Transform();
	wT.scale = m_Scale;
	wT.Rotate(m_Rotation);
	wT.position = m_WorldPos;

	return wT.GetTransform();
}

const glm::mat4 Pipeline::GetViewTransform() {
	return m_pCamera->m_Transform.GetTransform();
}

const glm::mat4 Pipeline::GetProjTransform() {
	return glm::perspectiveFovLH(glm::radians(m_PersProps.FOV), m_PersProps.Width, m_PersProps.Height, m_PersProps.zNear, m_PersProps.zFar);
}
