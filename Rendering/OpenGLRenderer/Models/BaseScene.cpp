#include "BaseScene.h"

using glm::vec3;

BaseScene::BaseScene() {
	Log::Info("Creating test model...");
	mNyen = new BaseModel("./Rendering/Resources/models/nyen/nyen plush.obj");
	mModels.push_back(mNyen);

	InstancedModel* testModel2 = new InstancedModel("./Rendering/Resources/models/survivalBackpack/backpack.obj");
	testModel2->mTransform->Translate(vec3(0.0f, 0.0f, 10.0f));
	testModel2->AddInstance(vec3(0.0f, 5.0f, 0.0f));
	testModel2->AddInstance(vec3(5.0f, 10.0f, 0.0f));
	testModel2->AddInstance(vec3(-3.0f, -5.0f, 0.0f));
	testModel2->mInstanceTransforms[1].Rotate(vec3(0.0f,glm::radians(30.0f),2.0f));
	mModels.push_back(testModel2);

	Log::Info("Instantiating lights...");
	auto light1 = new PointLight();
	light1->mTransform->Translate(vec3(3.0f, 1.0f, 3.0f));
	light1->mLightColor = vec3(1.0f, 1.0f, 1.0f);
	mLights.push_back(light1);
	auto light2 = new PointLight();
	light2->mTransform->Translate(vec3(0.0f, -2.0f, 1.0f));
	light2->mLightColor = vec3(1.0f, 1.0f, 1.0f);
	mLights.push_back(light2);
	auto light3 = new PointLight();
	light3->mTransform->Translate(vec3(2.0f, 4.0f, 6.0f));
	light3->mLightColor = vec3(1.0f, 0.2f, 0.2f);
	mLights.push_back(light3);
	auto light4 = new PointLight();
	light4->mTransform->Translate(vec3(0.0f, 6.0f, 9.0f));
	light4->mLightColor = vec3(0.2f, 0.2f, 1.0f);
	mLights.push_back(light4);
	auto light5 = new PointLight();
	light5->mTransform->Translate(vec3(4.0f, -2.0f, 12.0f));
	light5->mLightColor = vec3(1.0f, 1.0f, 1.0f);
	mLights.push_back(light5);
}

void BaseScene::LoadFromFile(std::string path) {}

void BaseScene::Tick(float deltaTime) {
	auto yawDiff = glm::radians(360.0f) * deltaTime;
	//Log::Info("Yaw diff: " + std::to_string(yawDiff));
	//Log::Info("Nyen orientation: " + Log::ToString(mNyen->mTransform->orientation));
	mNyen->mTransform->Rotate(yawDiff, vec3(0,1,0));
	//mNyen->mTransform->Rotate(vec3(0.0f,yawDiff,0.0f));
	
	//Log::Info("Scene delta time: " + std::to_string(deltaTime));
}