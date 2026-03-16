#include "OpenGLRenderer.h"

#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#pragma warning(pop)

#define VIEW_WIDTH this->m_currentSettings.window.windowWidth
#define VIEW_HEIGHT this->m_currentSettings.window.windowHeight

OpenGLRenderer* OpenGLRenderer::m_pInstance = nullptr;

OpenGLRenderer::OpenGLRenderer() {
	m_GeomPassTech = GeomPassTech();
}

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

	//Log::Info("Preparing techniques...");
	//if (!m_GeomPassTech.Init()) {
	//	throw std::runtime_error("Failed to initialise geometry pass technique");
	//}

	//m_GeomPassTech.Enable();
	//m_GeomPassTech.SetColorTexUnit(COLOR_TEXTURE_UNIT_INDEX);

	Log::Info("Loading shaders...");
	// Prepare shaders
	LoadShaders();

	Log::Info("Creating G-Buffer...");
	m_pGBuffer = new GBuffer();
	m_pGBuffer->Init(VIEW_WIDTH, VIEW_HEIGHT);


	Log::Info("Creating uniform buffer object...");
	float aspectRatio = VIEW_WIDTH / static_cast<float>(VIEW_HEIGHT);
	projectionMatrix = glm::perspective(glm::radians(70.0f), aspectRatio, m_currentSettings.graphics.clipPlaneNear, m_currentSettings.graphics.clipPlaneFar);
	sUBO initData = {
		m_pCamera->m_Transform.GetTransform(),
		m_pCamera->m_Transform.position,
		projectionMatrix
	};
	m_pUBO = new UniformBufferObject(initData);

	Log::Info("Creating test model...");
	BaseModel* testModel = new BaseModel("./Rendering/Resources/models/survivalBackpack/backpack.obj");
	testModel->m_pShader = getShaderByName("baseShader");
	LoadedModels.push_back(testModel);

	BaseModel* groundPlane = new BaseModel("./Rendering/Resources/models/Primitives/primitivePlane.obj");
	groundPlane->m_pShader = getShaderByName("baseShader");
	groundPlane->m_pTransform->Translate(glm::vec3(0.0f, -10.0f, 0.0f));
	groundPlane->m_pTransform->Scale(glm::vec3(10.0f));
	LoadedModels.push_back(groundPlane);
	
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
		if (fpsPrintDelta > 100) {
			fpsPrintDelta = 0;
			Log::Info(std::to_string(m_elapsedRenderTime) + " elapsed | " + std::to_string(m_deltaRenderTime) + " delta | " + Utilities::calculateFPS(m_deltaRenderTime, 2) + "FPS");
		};
		fpsPrintDelta += m_deltaRenderTime;

		if (m_shouldRender) {
			LoadedShaders[0]->setUniformVec4("ourColor", 1.0f, static_cast<float>(sin(m_elapsedRenderTime)), 1.0f, 1.0f);
			UpdateUniformBuffers();

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// [Geometry Pass] //
			//-----------------//
			DSPassGeometry();
			renderQuad();
			DSPassLighting();
			
			// [Finalise Tick] //
			//-----------------//
			glfwSwapBuffers(windowInstance);
		}
		glfwPollEvents();
	}

	m_pInstance->Cleanup();
}

void OpenGLRenderer::UpdateUniformBuffers() {
	glm::mat4 viewMatrix = m_pCamera->m_Transform.GetTransform();
	glm::vec3 viewPos = m_pCamera->m_Transform.position;
	sUBO newData = {
		viewMatrix,
		viewPos,
		projectionMatrix
	};
	m_pUBO->UploadNewData(newData);
}

void OpenGLRenderer::Cleanup() {
	m_state = OpenGLRendererState::CLEANUP;
	Log::Info("Cleaning up...");
	glfwTerminate();
}


// Deferred Shading

//PerspectiveProperties defaultPersProps = {
//	.FOV = 70.0f,
//	.Width = 1280,
//	.Height = 720,
//	.zNear = 0.1f,
//	.zFar = 100
//};

void OpenGLRenderer::DSPassGeometry() {
	//m_GeomPassTech.Enable();

	m_pGBuffer->BindForWrite();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Pipeline p;
	//p.Scale(glm::vec3(0.1f));
	//p.Rotate(0.0f, 0.0f, 0.0f);
	//p.WorldPos(-0.8f, -1.0f, 12.0f);
	//p.SetCamera(m_pCamera);
	//p.SetPerspectiveProperties(defaultPersProps);
	//sUBO newData = {
	//	.viewMatrix = p.GetViewTransform(),
	//	.viewPosition = m_pCamera->m_Transform.position,
	//	.perspectiveMatrix = p.GetProjTransform()
	//};
	//m_pUBO->UploadNewData(newData);

	// Draw models
	for (int i = 0; i < LoadedModels.size(); i++) {
		LoadedModels[i]->DrawModel();
	}
}

void OpenGLRenderer::DSPassLighting() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_pGBuffer->BindForRead();

	GLsizei halfWidth = (GLsizei)(VIEW_WIDTH / 2.0f);
	GLsizei halfHeight = (GLsizei)(VIEW_HEIGHT / 2.0f);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_POSITION);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_DIFFUSE);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, 0, halfHeight, halfWidth, VIEW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_NORMAL);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, halfWidth, halfHeight, VIEW_WIDTH, VIEW_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	m_pGBuffer->SetReadBuffer(GBuffer::GBUFFER_TEXTURE_TEXCOORD);
	glBlitFramebuffer(0, 0, VIEW_WIDTH, VIEW_HEIGHT, halfWidth, 0, VIEW_WIDTH, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}