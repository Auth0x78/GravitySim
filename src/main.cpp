#pragma region Main Headers
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <vector>
#include <iostream>
#pragma endregion
#pragma region GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma endregion
#pragma region My Library Includes
#include <VAO.h>
#include <VBO.h>
#include <EBO.h>
#include <shader.h>
#include <sphere.h>
#pragma endregion


#define USE_GPU_ENGINE 1
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}

glm::vec3 cameraPos = glm::vec3(0, -0.25f, 3.0f);  // Position
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Direction (Looking forward)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // Up vector

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f;  // Time of last frame


static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	float cameraSpeed = 2.5f * deltaTime; // Adjust based on frame time
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);

	static float lastX = width / 2.0f, lastY = height / 2.0f;
	static float yaw = -90.0f, pitch = 0.0f;

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // Inverted
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 2.0f * deltaTime;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;
	pitch = glm::clamp(pitch, -89.0f, 89.0f); // Prevent flipping

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

float map(int value, int a, int b, float c, float d) {
	return c + (d - c) * (float(value - a) / float(b - a));
}

int main(void)
{

	if (!glfwInit())
		return -1;


#pragma region report opengl errors to std
	//enable opengl debugging output.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#pragma endregion


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //you might want to do this when testing the game for shipping


	GLFWwindow *window = window = glfwCreateWindow(640, 480, "Gravity Sim", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


#pragma region report opengl errors to std
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#pragma endregion

#pragma region Define Camera Properties
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);

	// View Matrix (Camera Transformation)
	glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	// Projection Matrix (Perspective)
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 300.0f);

#pragma endregion

	Sphere sphere0(1.0f, 10, 10);

	//shader loading example
	Shader mainShader(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag");

	mainShader.Activate();
	mainShader.SetUniformMatrix4fv("view", view);
	mainShader.SetUniformMatrix4fv("projection", projection);
	
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime(); // Get current time in seconds
		deltaTime = currentFrame - lastFrame; // Compute delta time
		lastFrame = currentFrame; // Update last frame time

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// View Matrix (Camera Transformation)
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Projection Matrix (Perspective)
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 300.0f);

		mainShader.SetUniformMatrix4fv("view", view);
		mainShader.SetUniformMatrix4fv("projection", projection);

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		sphere0.Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//there is no need to call the clear function for the libraries since the os will do that for us.
	//by calling this functions we are just wasting time.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
