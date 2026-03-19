
#include <iostream>
#include <map>
#include <thread>
#include <string>

#include "Rendering/OpenGLRenderer/OpenGLRenderer.h"

Settings *settings = new Settings();

struct sInput {
	bool keyW = false;
	bool keyA = false;
	bool keyS = false;
	bool keyD = false;
	bool keyQ = false;
	bool keyE = false;
	bool keyP = false;
	double lastMouseX = settings->window.windowWidth / 2;
	double lastMouseY = settings->window.windowHeight / 2;
};
sInput input;
bool firstMouseInput = true;

bool pKeyDebounce = false;

void enableInputProcessing(OpenGLRenderer* pRenderer)
{
	Window* pWindow = pRenderer->GetWindow();
	GLFWwindow* pGLFWWindow = pWindow->GetWindow();
	BaseCamera* pCamera = pRenderer->GetCamera();

	glfwSetInputMode(pGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	while (pRenderer->GetState() == OpenGLRendererState::RUNNING) {
		// Process inputs
		if (glfwGetKey(pGLFWWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(pGLFWWindow, GLFW_TRUE);
		}

		input.keyW = (glfwGetKey(pGLFWWindow, GLFW_KEY_W) == GLFW_PRESS);
		input.keyA = (glfwGetKey(pGLFWWindow, GLFW_KEY_A) == GLFW_PRESS);
		input.keyS = (glfwGetKey(pGLFWWindow, GLFW_KEY_S) == GLFW_PRESS);
		input.keyD = (glfwGetKey(pGLFWWindow, GLFW_KEY_D) == GLFW_PRESS);
		input.keyQ = (glfwGetKey(pGLFWWindow, GLFW_KEY_Q) == GLFW_PRESS);
		input.keyE = (glfwGetKey(pGLFWWindow, GLFW_KEY_E) == GLFW_PRESS);
		input.keyP = (glfwGetKey(pGLFWWindow, GLFW_KEY_P) == GLFW_PRESS);

		glm::vec3 dirInput = glm::vec3(0.0f);
		if (input.keyW) dirInput.z = 1;
		if (input.keyA) dirInput.x = -1;
		if (input.keyS) dirInput.z = -1;
		if (input.keyD) dirInput.x = 1;
		if (input.keyQ) dirInput.y = -1;
		if (input.keyE) dirInput.y = 1;

		if (!pKeyDebounce && input.keyP) {
			pRenderer->ToggleWireframe();
			pKeyDebounce = true;
		}
		else if (pKeyDebounce && !input.keyP) {
			pKeyDebounce = false;
		}

		glm::vec3 angInput = glm::vec3(0.0f);
		double currentMouseX, currentMouseY;
		glfwGetCursorPos(pGLFWWindow, &currentMouseX, &currentMouseY);
		if (firstMouseInput) {
			input.lastMouseX = currentMouseX;
			input.lastMouseY = currentMouseY;
			firstMouseInput = false;
		}

		glm::vec2 mouseDelta = glm::vec2(currentMouseX - input.lastMouseX, input.lastMouseY - currentMouseY);

		angInput = glm::vec3(mouseDelta.y, mouseDelta.x, 0.0f);
		input.lastMouseX = currentMouseX;
		input.lastMouseY = currentMouseY;

		pCamera->Move(dirInput, angInput);

		// Sleep for a bit to reduce CPU usage
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void runRenderer(OpenGLRenderer* pRenderer) {
	try
	{
		pRenderer->Init(settings);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		system("pause");
	}

	OpenGLRenderer::DestroyInstance();
}

int main() {
	std::cout << "Initialising Refraction...\n";

	settings->window.windowWidth = 1280;
	settings->window.windowHeight = 720;
	settings->graphics.viewportWidth = 1280;
	settings->graphics.viewportHeight = 720;
	settings->controls.cameraSensitivity = 0.5f;
	settings->controls.cameraSpeed = 0.5f;

	std::cout << "Starting render thread...\n";
	OpenGLRenderer* renderer = OpenGLRenderer::GetInstance();
	std::thread renderThread(runRenderer, renderer);
	
	// Wait for rendering engine to start running
	while (renderer->GetState() != OpenGLRendererState::RUNNING) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::cout << "Starting input thread...\n";
	// Allow inputs to be processed
	std::thread inputThread(enableInputProcessing, renderer);
	
	// Wait for rendering engine to clean up before exiting
	while (renderer->GetState() != OpenGLRendererState::EXIT) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	
	renderThread.join();
	inputThread.join();
	
	std::cout << "\n\n--------------------------------" << std::endl;
	std::cout << "Refraction shut down successfully.\n\n";
	system("pause");
	
	return EXIT_SUCCESS;
}