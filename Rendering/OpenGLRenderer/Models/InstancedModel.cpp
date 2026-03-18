#include "InstancedModel.h"

InstancedModel::InstancedModel(std::string modelSourcePath) : BaseModel(modelSourcePath) {
}

void InstancedModel::AddInstance(glm::vec3 relativePos) {
	Transform newTransform = Transform();
	newTransform.position = relativePos;

	mTransforms.push_back(newTransform);
}

void InstancedModel::DrawModel() {

	m_pShader->Activate();
	for (auto& transform : mTransforms) {
		auto matrix = transform.GetTransform();
		matrix *= m_pTransform->GetTransform();
		m_pShader->setUniformMat4("modelTransform", matrix);

		DrawMeshesRaw();
		Log::Info("Drew instance of model");
	}
}
