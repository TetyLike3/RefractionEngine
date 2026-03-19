#include "OpenGLRenderer.h"

#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#pragma warning(pop)

#define VIEW_WIDTH this->mCurrentSettings.window.windowWidth
#define VIEW_HEIGHT this->mCurrentSettings.window.windowHeight
constexpr auto FPS_INTERVAL = 500;

OpenGLRenderer* OpenGLRenderer::mInstance = nullptr;

OpenGLRenderer::OpenGLRenderer() {}

OpenGLRenderer* OpenGLRenderer::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new OpenGLRenderer();
		Log::Info("Instance created");
	}
	return mInstance;
}

void OpenGLRenderer::DestroyInstance()
{
	Log::Info("Instance destruction requested");
	if (mInstance != nullptr)
	{
		if (mInstance->GetState() != OpenGLRendererState::CLEANUP) {
			mInstance->Cleanup();
		}

		Log::Info("Exiting...");
		mInstance->mState = OpenGLRendererState::EXIT;
	}
}

std::vector<unsigned int> VAOs = {};
std::vector<unsigned int> VBOs = {};

glm::mat4 projectionMatrix;

int OpenGLRenderer::Init(Settings* initSettings) {
	mState = OpenGLRendererState::INIT;
	Log::Info("Initializing...");
	mCurrentSettings = *initSettings;

	Log::Info("Instantiating window...");
	glfwInit();
	mWindow = new Window();
	mWindow->Init(initSettings->window);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	Log::Info("Instantiating camera...");
	mCamera = new BaseCamera();
	mCamera->SetCameraSpeed(mCurrentSettings.controls.cameraSpeed);
	mCamera->SetCameraSensitivity(mCurrentSettings.controls.cameraSensitivity);

	Log::Info("Loading shaders...");
	ShaderManager::LoadAllShaders();

	mGeomPassShader = ShaderManager::GetShaderByName("gbufferShader");
	mLightingPassShader = ShaderManager::GetShaderByName("lightingShader");

	mLightingPassShader->Activate();
	mLightingPassShader->setUniformInt("gPosition", 0);
	mLightingPassShader->setUniformInt("gNormal", 1);
	mLightingPassShader->setUniformInt("gAlbedoSpec", 2);

	Log::Info("Creating G-Buffer...");
	mGBuffer = new GBuffer();
	mGBuffer->Init(VIEW_WIDTH, VIEW_HEIGHT);

	Log::Info("Creating uniform buffer object...");
	float aspectRatio = VIEW_WIDTH / static_cast<float>(VIEW_HEIGHT);
	projectionMatrix = glm::perspective(glm::radians(mCamera->FOVy), aspectRatio, mCurrentSettings.graphics.clipPlaneNear, mCurrentSettings.graphics.clipPlaneFar);
	sUBO initData = {
		mCamera->GetViewMatrix(),
		projectionMatrix
	};
	mUBO = new UniformBufferObject(initData);

	Log::Info("Loading test scene...");
	mLoadedScene = new BaseScene();
	
	Log::Info("Initialisation complete");

	mStartRenderTime = glfwGetTime();
	MainLoop();

	return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void OpenGLRenderer::MainLoop() {
	mState = OpenGLRendererState::RUNNING;
	double fpsPrintDelta = 0;
	double frameCounter = 0;
	GLFWwindow* windowInstance = mWindow->GetWindow();

	while (!glfwWindowShouldClose(windowInstance))
	{
		mDeltaRenderTime = glfwGetTime() - mElapsedRenderTime;
		mElapsedRenderTime = glfwGetTime() - mStartRenderTime;
		if (fpsPrintDelta > FPS_INTERVAL) {
			fpsPrintDelta = 0;
			Log::Info(std::to_string(mElapsedRenderTime) + " elapsed | " + std::to_string(mDeltaRenderTime) + " delta | " + Utilities::calculateFPS(frameCounter/FPS_INTERVAL, 2) + "FPS");
			frameCounter = 0;
			//auto cameraPos = mCamera->m_Transform.position;
			//Log::Info("Camera pos: " + Log::ToString(cameraPos));
			//auto cameraForward = mCamera->m_Transform.GetForwardVector();
			//Log::Info("Camera forward: " + Log::ToString(cameraForward));
		};
		fpsPrintDelta += mDeltaRenderTime;
		frameCounter++;

		if (mShouldRender) {
			UpdateUniformBuffers();
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// [Geometry Pass] //
			//-----------------//
			DSPassGeometry();
			DSPassLighting();
			DSPassFinal();

			// [Finalise Tick] //
			//-----------------//
			glfwSwapBuffers(windowInstance);
		}
		glfwPollEvents();
		mLoadedScene->Tick(mDeltaRenderTime/1000);
	}

	mInstance->Cleanup();
}

void OpenGLRenderer::UpdateUniformBuffers() {
	sUBO newData{};
	newData.viewMatrix = mCamera->GetViewMatrix();
	newData.perspectiveMatrix = projectionMatrix;
	mUBO->UploadNewData(newData);
}

void OpenGLRenderer::Cleanup() {
	mState = OpenGLRendererState::CLEANUP;
	Log::Info("Cleaning up...");
	glfwTerminate();
}


// Deferred Shading

void OpenGLRenderer::DSPassGeometry() {
	mGBuffer->BindAny();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mGeomPassShader->Activate();

	// Draw models
	for (int i = 0; i < mLoadedScene->mModels.size(); i++) {
		mLoadedScene->mModels[i]->DrawModel();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::DSPassLighting() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mGBuffer->BindTextures();

	mLightingPassShader->Activate();
	for (auto i = 0; i < mLoadedScene->mLights.size(); i++) {
		auto light = mLoadedScene->mLights[i];
		light->UpdateShaderUniforms(i);
	}
	mLightingPassShader->setUniformVec3("viewPos", mCamera->m_Transform.position);
	// finally render quad
	renderQuad();
}

void OpenGLRenderer::DSPassFinal() {
	mGBuffer->BindForRead();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}