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
#include "Graphics/Window.h"
#include "Graphics/BaseShader.h"
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
	OpenGLRendererState GetState() { return m_state; }
	Window* GetWindow() { return m_pWindow; }
	BaseCamera* GetCamera() { return m_pCamera; }
	int Init(Settings* initSettings);
private:
	OpenGLRenderer();
	void LoadShaders();
	void MainLoop();
	void UpdateUniformBuffers();
	void Cleanup();

	// Deferred shading functions
	void DSPassGeometry();
	void DSPassLighting();
	void DSPassFinal();

	static OpenGLRenderer* m_pInstance;
	OpenGLRendererState m_state = OpenGLRendererState::NONE;
	Settings m_currentSettings;
	Window* m_pWindow = nullptr;

	UniformBufferObject* m_pUBO = nullptr;
	BaseShader* m_pGeomPassShader = nullptr;
	BaseShader* m_pLightingPassShader = nullptr;
	GBuffer* m_pGBuffer = nullptr;

	BaseCamera* m_pCamera = nullptr;
	vector<PointLight*> m_pScenePointLights = {};

	bool m_shouldRender = true;
	double m_startRenderTime = 0.0f;
	double m_elapsedRenderTime = 0.0f;
	double m_deltaRenderTime = 0.0f;
};

inline void nativeDebugPrint(std::string message, bool newLine = false) { std::cout << (newLine ? "\n" : "") << "OpenGLRenderer DEBUG - " << message << std::endl; }