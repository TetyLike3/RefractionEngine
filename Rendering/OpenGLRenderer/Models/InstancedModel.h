#pragma once
#include "BaseModel.h"

class InstancedModel : public BaseModel {
public:
	std::vector<Transform> mInstanceTransforms = {};

	InstancedModel(std::string modelSourcePath);

	void AddInstance(glm::vec3 relativePos);
	void DrawModel();
private:
};
