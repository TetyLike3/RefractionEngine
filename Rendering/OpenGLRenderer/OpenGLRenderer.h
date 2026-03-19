#pragma once

#include <iostream>
#include <map>
#include <thread>
#include <vector>

#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include "Constants.h"
#include "Log.h"
#include "Utilities.h"
#include "Settings.h"
#include "ShaderManager.h"
#include "Graphics/Window.h"
#include "Graphics/BaseShader.h"
#include "Models/BaseScene.h"
#include "Models/BaseModel.h"
#include "Models/InstancedModel.h"
#include "Models/BaseCamera.h"
#include "Models/BaseLight.h"
#include "Models/PointLight.h"
#include "Graphics/Buffers/UniformBufferObject.h"
#include "Graphics/Buffers/GBuffer.h"


enum class OpenGLRendererState
{
	NONE,
	INIT,
	RUNNING,
	CLEANUP,
	EXIT
};

struct sSettings {
	struct sControlSettings {
		float cameraSensitivity = .1f; // Sensitivity of the camera movement.
		float cameraSpeed = 0.05f; // Speed of the camera movement.
	} control;
};


class OpenGLRenderer
{
public:
	static OpenGLRenderer* GetInstance();
	static void DestroyInstance();
	OpenGLRendererState GetState() { return mState; }
	Window* GetWindow() { return mWindow; }
	BaseCamera* GetCamera() { return mCamera; }
	int Init(Settings* initSettings);
	void ToggleWireframe();
private:
	OpenGLRenderer();
	void MainLoop();
	void UpdateUniformBuffers();
	void Cleanup();

	// Deferred shading functions
	void DSPassGeometry();
	void DSPassLighting();
	void DSPassFinal();

	static OpenGLRenderer* mInstance;
	OpenGLRendererState mState = OpenGLRendererState::NONE;
	Settings mCurrentSettings;
	Window* mWindow = nullptr;

	UniformBufferObject* mUBO = nullptr;
	BaseShader* mGeomPassShader = nullptr;
	BaseShader* mLightingPassShader = nullptr;
	GBuffer* mGBuffer = nullptr;

	BaseCamera* mCamera = nullptr;
	BaseScene* mLoadedScene = nullptr;

	bool mShouldRender = true;
	bool mWireframeMode = false;
	double mStartRenderTime = 0.0f;
	double mElapsedRenderTime = 0.0f;
	double mDeltaRenderTime = 0.0f;
};
