#include "OpenGLRenderer.h"

#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#pragma warning(pop)

#define VIEW_WIDTH this->m_currentSettings.window.windowWidth
#define VIEW_HEIGHT this->m_currentSettings.window.windowHeight
constexpr auto FPS_INTERVAL = 300;

OpenGLRenderer* OpenGLRenderer::m_pInstance = nullptr;

OpenGLRenderer::OpenGLRenderer() {}

OpenGLRenderer* OpenGLRenderer::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new OpenGLRenderer();
		Log::Info("Instance created");
	}
	return m_pInstance;
}

void OpenGLRenderer::DestroyInstance()
{
	Log::Info("Instance destruction requested");
	if (m_pInstance != nullptr)
	{
		if (m_pInstance->GetState() != OpenGLRendererState::CLEANUP) {
			m_pInstance->Cleanup();
		}

		Log::Info("Exiting...");
		m_pInstance->m_state = OpenGLRendererState::EXIT;
	}
}

std::string shadersPath = "Rendering/Resources/shaders";
std::vector<BaseShader*> LoadedShaders = {};

void OpenGLRenderer::LoadShaders() {
	using std::string, std::vector, std::filesystem::directory_entry;

	vector<directory_entry> shaderSources = Utilities::getFoldersInFolder(shadersPath);

	for (const auto& shaderSource : shaderSources) {
		string shaderSourcePath = shaderSource.path().string();
		Log::Info("Loading shader source: " + shaderSourcePath);
		BaseShader* newShader = new BaseShader(shaderSourcePath);
		LoadedShaders.push_back(newShader);
	}
}

BaseShader* getShaderByName(const std::string name) {
	for (const auto& shader : LoadedShaders) {
		if (shader->m_name == name) {
			return shader;
		}
	}
	throw std::runtime_error("Could not get shader of name " + name);
}

std::vector<unsigned int> VAOs = {};
std::vector<unsigned int> VBOs = {};
std::vector<BaseModel*> LoadedModels = {};

glm::mat4 projectionMatrix;

int OpenGLRenderer::Init(Settings* initSettings) {
	m_state = OpenGLRendererState::INIT;
	Log::Info("Initializing...");
	m_currentSettings = *initSettings;

	Log::Info("Instantiating window...");
	glfwInit();
	m_pWindow = new Window();
	m_pWindow->Init(initSettings->window);

	glEnable(GL_DEPTH_TEST);

	Log::Info("Instantiating camera...");
	m_pCamera = new BaseCamera();
	m_pCamera->setCameraSpeed(m_currentSettings.controls.cameraSpeed);
	m_pCamera->setCameraSensitivity(m_currentSettings.controls.cameraSensitivity);

	Log::Info("Loading shaders...");
	// Prepare shaders
	LoadShaders();

	m_pGeomPassShader = getShaderByName("gbufferShader");

	Log::Info("Creating G-Buffer...");
	m_pGBuffer = new GBuffer();
	m_pGBuffer->Init(VIEW_WIDTH, VIEW_HEIGHT);


	Log::Info("Creating uniform buffer object...");
	float aspectRatio = VIEW_WIDTH / static_cast<float>(VIEW_HEIGHT);
	projectionMatrix = glm::perspective(glm::radians(m_pCamera->FOVy), aspectRatio, m_currentSettings.graphics.clipPlaneNear, m_currentSettings.graphics.clipPlaneFar);
	sUBO initData = {
		m_pCamera->m_Transform.GetTransform(),
		m_pCamera->m_Transform.position,
		projectionMatrix
	};
	m_pUBO = new UniformBufferObject(initData);

	Log::Info("Creating test model...");
	BaseModel* testModel = new BaseModel("./Rendering/Resources/models/survivalBackpack/backpack.obj");
	testModel->m_pShader = m_pGeomPassShader;
	testModel->m_pTransform->Translate(glm::vec3(0.0f, 0.0f, 10.0f));
	LoadedModels.push_back(testModel);

	BaseModel* testModel2 = new BaseModel("./Rendering/Resources/models/survivalBackpack/backpack.obj");
	testModel2->m_pShader = m_pGeomPassShader;
	testModel2->m_pTransform->Translate(glm::vec3(0.0f, -10.0f, 0.0f));
	testModel2->m_pTransform->Scale(glm::vec3(10.0f));
	LoadedModels.push_back(testModel2);
	
	Log::Info("Initialisation complete");

	m_startRenderTime = glfwGetTime();
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
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
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
	m_state = OpenGLRendererState::RUNNING;
	double fpsPrintDelta = 0;
	GLFWwindow* windowInstance = m_pWindow->GetWindow();

	while (!glfwWindowShouldClose(windowInstance))
	{
		m_deltaRenderTime = glfwGetTime() - m_elapsedRenderTime;
		m_elapsedRenderTime = glfwGetTime() - m_startRenderTime;
		if (fpsPrintDelta > FPS_INTERVAL) {
			fpsPrintDelta = 0;
			Log::Info(std::to_string(m_elapsedRenderTime) + " elapsed | " + std::to_string(m_deltaRenderTime) + " delta | " + Utilities::calculateFPS(m_deltaRenderTime, 2) + "FPS");
		};
		fpsPrintDelta += m_deltaRenderTime;

		if (m_shouldRender) {
			UpdateUniformBuffers();

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// TODO: models currently use their own shaders which are built on forward shading (i think) but to implement deferred shading i think we need a single global shader to render each buffer so uhh yea figure that out ty <3
			// hi ok

			// [Geometry Pass] //
			//-----------------//
			DSPassGeometry();
			//renderQuad();
			//DSPassLighting();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			
			m_pGBuffer->BindForRead();
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, VIEW_WIDTH, VIEW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// [Finalise Tick] //
			//-----------------//
			glfwSwapBuffers(windowInstance);
		}
		glfwPollEvents();
	}

	m_pInstance->Cleanup();
}

void OpenGLRenderer::UpdateUniformBuffers() {
	sUBO newData{};
	newData.viewMatrix = m_pCamera->m_Transform.GetTransform();
	newData.viewPosition = m_pCamera->m_Transform.position;
	newData.perspectiveMatrix = projectionMatrix;
	m_pUBO->UploadNewData(newData);
}

void OpenGLRenderer::Cleanup() {
	m_state = OpenGLRendererState::CLEANUP;
	Log::Info("Cleaning up...");
	glfwTerminate();
}


// Deferred Shading

void OpenGLRenderer::DSPassGeometry() {
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pGBuffer->BindAny();
	m_pGeomPassShader->Activate();

	// Draw models
	for (int i = 0; i < LoadedModels.size(); i++) {
		LoadedModels[i]->DrawModel();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderer::DSPassLighting() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pGBuffer->BindForRead();

	GLsizei halfWidth = (GLsizei)(VIEW_WIDTH / 2.0f);
	GLsizei halfHeight = (GLsizei)(VIEW_HEIGHT / 2.0f);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_POSITION);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_NORMAL);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, halfHeight, halfWidth, VIEW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_COLORSPECULAR);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, halfWidth, halfHeight, VIEW_WIDTH, VIEW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}