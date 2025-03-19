#include "Game.h"

#define USE_GPU_ENGINE 1
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

//static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//
//	float cameraSpeed = 2.5f * deltaTime; // Adjust based on frame time
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		cameraPos += cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		cameraPos -= cameraSpeed * cameraFront;
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//}
//
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//	int width = 0, height = 0;
//	glfwGetFramebufferSize(window, &width, &height);
//
//	static float lastX = width / 2.0f, lastY = height / 2.0f;
//	static float yaw = -90.0f, pitch = 0.0f;
//
//	float xOffset = xpos - lastX;
//	float yOffset = lastY - ypos; // Inverted
//	lastX = xpos;
//	lastY = ypos;
//
//	float sensitivity = 2.0f * deltaTime;
//	xOffset *= sensitivity;
//	yOffset *= sensitivity;
//
//	yaw += xOffset;
//	pitch += yOffset;
//	pitch = glm::clamp(pitch, -89.0f, 89.0f); // Prevent flipping
//
//	glm::vec3 direction;
//	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//	direction.y = sin(glm::radians(pitch));
//	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//	cameraFront = glm::normalize(direction);
//}

int main(void) {

	int width = 0, height = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
		printf("Could not initialize SDL: %s.\n", SDL_GetError());
		return -1;
	}

	SDL_Window *pWindow = SDL_CreateWindow("Gravity Sim", 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (pWindow == nullptr) {
		SDL_Log("Failed to create SDL3 window: %s", SDL_GetError());
		// Handle error appropriately (exit, cleanup, etc.)
	}

	SDL_GLContext glContext = SDL_GL_CreateContext(pWindow);
	if (glContext == nullptr) {
		SDL_Log("Failed to create OpenGL context: %s", SDL_GetError());
		// Handle error (e.g., cleanup and exit)
	}

#pragma region report opengl errors to std
	//enable opengl debugging output.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#pragma endregion

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_MakeCurrent(pWindow, glContext);
	gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
	SDL_GL_SetSwapInterval(1);

#pragma region report opengl errors to std
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#pragma endregion

	// Set relative mouse mode
	SDL_SetWindowRelativeMouseMode(pWindow, true); // Enable relative mouse mode

	// Get Window width and height
	SDL_GetWindowSizeInPixels(pWindow, &width, &height);

	Sphere sphere0(1.0f, 10, 10);

	//shader loading example
	Shader mainShader(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag");

	mainShader.Activate();
	/*
	mainShader.SetUniformMatrix4fv("view", view);
	mainShader.SetUniformMatrix4fv("projection", projection);
	*/

	// Timing variables
	Uint64 NOW = SDL_GetPerformanceCounter();
	Uint64 LAST = 0;
	double deltaT;

	// Main Game Class
	Game mainGame(pWindow, glContext);

	bool running = true;
	while (running) {
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		// Calculate deltaTime (time elapsed between frames)
		deltaT = (double)(NOW - LAST) / SDL_GetPerformanceFrequency();

		// Update deltaTime calculations
		mainGame.deltaTime = deltaT;
		mainGame.elapsedTime += deltaT;

		// Poll Window Events
		mainGame.PollEvents();
		
		// Run the actual Game & UI
		running = mainGame.GameLoop();
		mainGame.RenderUI();

		// Swap frame buffers
		SDL_GL_SwapWindow(pWindow);
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
	return 0;
}
