#pragma once
#include "BaseModel.h"

class InstancedModel : public BaseModel {
public:
	InstancedModel(std::string modelSourcePath);

	void AddInstance(glm::vec3 relativePos);
	void DrawModel();
private:
	std::vector<Transform> mTransforms = {};
};
