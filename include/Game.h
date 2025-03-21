#pragma once

#define KG_TO_GMASS 	1.0e-6
#define METER_TO_GLEN	1.0e-9
#define KM_TO_GLEN		(1000.0 * METER_TO_GLEN)
#define SEC_TO_GSEC 	1.0e-6

// DISPLAYED Units:
// MASS: m [kg]
// RADIUS: r [km]
// POSITION: P [10^3 km]
// VELOCITY: V [km/s]

// Conversion To Game Units:
// MASS: m [kg] -> m [kg] * kg2gm [gm = Game Mass Unit]
// LENGTH: r [km] -> (r * 1000)[m] * meter2gl [gl = Game Length Unit]
// TIME: [s] -> [s] * sec2gs [gs = Game Time Unit]
// POSITION: P [10^3 km]-> (P * 10^3 * 10^3)[m] * meter2gl [gl]
// VELOCITY: V [km/s] -> (V * 10^3 [m] * meter2gl) / ([s] * sec2gs) [gl/gs]


// G in game units will be = G * meter2gl^3 / (kg2gm * sec2gs^2)
constexpr double G = (6.67430e-11 * METER_TO_GLEN * METER_TO_GLEN * METER_TO_GLEN) / (KM_TO_GLEN * SEC_TO_GSEC * SEC_TO_GSEC);// In Game Units

#pragma region Main Headers
#include <glad/glad.h>
#include <openglDebug.h>
#include <vector>
#include <iostream>
#pragma endregion

#pragma region SDL3 Headers
#include <SDL3/SDL.h>
#include <SDL3/SDL_mouse.h>
#pragma endregion

#pragma region IMGui Includes
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#pragma endregion

#pragma region GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#pragma endregion

#pragma region My Library Includes
#include <shader.h>
#include <sphere.h>
#pragma endregion

struct Planet {
	double mass;
	double radius;
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 material;

	Sphere renderer;
	char name[16];

	Planet(double m, double r, float p[3], float v[3], float mat[3], const char nameData[16])
		: mass(m), radius(r), position(glm::zero<glm::vec3>()), velocity(glm::zero<glm::vec3>()), material(glm::zero<glm::vec3>()), renderer(r, 10, 10) {
		// Set Variables
		position = glm::vec3(p[0], p[1], p[2]);
		velocity = glm::vec3(v[0], v[1], v[2]);
		material = glm::vec3(mat[0], mat[1], mat[2]);

		renderer.SetPosition(position);
		// Copy the data from the passed array into the internal array.
		memcpy(name, nameData, 16);
	}

	// Default constructor (if needed by the container)
	Planet()
		: mass(0), radius(0),
		position(glm::vec3(0)), velocity(glm::vec3(0)),
		material(glm::vec3(0)), renderer(0, 10, 10)
	{
		name[0] = '\0';
	}

	// Move constructor
	Planet(Planet&& other) noexcept
		: mass(other.mass), radius(other.radius),
		position(std::move(other.position)), velocity(std::move(other.velocity)),
		material(std::move(other.material)), renderer(std::move(other.renderer))
	{
		std::copy(std::begin(other.name), std::end(other.name), std::begin(name));
	}

	// Delete copy operations if moving is desired
	Planet(const Planet&) = delete;
	Planet& operator=(const Planet&) = delete;

	// Move assignment operator
	Planet& operator=(Planet&& other) noexcept {
		if (this != &other) {
			mass = other.mass;
			radius = other.radius;
			position = std::move(other.position);
			velocity = std::move(other.velocity);
			material = std::move(other.material);
			renderer = std::move(other.renderer);
			std::copy(std::begin(other.name), std::end(other.name), std::begin(name));
		}
		return *this;
	}
};

class Game {
public:
	Game(SDL_Window* window, SDL_GLContext glContext);
	void PollEvents();
	bool GameLoop();
	void RenderUI();

	~Game();
	void Shutdown();

private:
	void ApplyGravity(Planet& pl1, Planet& pl2);
	void handleMouseEvent(SDL_Event& event);
	void handleKeyboard();

	enum KeyState {
		KEY_NOT_PRESSED = 0x00,
		KEY_UP   = 0x01,
		KEY_DOWN = 0x02,
		KEY_HELD = 0x03,
	};
	KeyState getKeyState(SDL_Scancode scancode);

public:
	// Time Calculation Variables
	double deltaTime = 0.0f;
	double elapsedTime = 0.0f;

private:
	SDL_Window* m_pWindow;
	SDL_GLContext m_GLContext;
	ImGuiIO* m_pIO;
	bool m_running = true;
	bool m_lookMode = false;
	bool m_runSim = false;
	float m_timeMultiplier = 1.0f;

	// SDL Property
	int m_width = 0;
	int m_height = 0;

	// Keyboard Variables
	Uint8 m_currentFrameKeyPress[SDL_SCANCODE_COUNT] = { 0 };
	Uint8 m_lastFrameKeyPress[SDL_SCANCODE_COUNT] = { 0 };
	
	// Main Shader
	Shader m_mainShader;

	// Camera Variables
	float cameraSpeed = 5.0f;
	glm::vec3 m_cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);  // Position
	glm::vec3 m_cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Direction (Looking forward)
	glm::vec3 m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);  // Up vector

	// View Matrix (Camera Transformation)
	glm::mat4 m_view;

	// Projection Matrix (Perspective)
	glm::mat4 m_projection;

	// Game Variables
	std::vector<Planet> m_vPlanets;

	// Add Planet Menu Variables
	double m_uiInputMass = 10;
	double m_uiInputRadius = 1.0;
	float  m_uiInputPos[3] = {0.0f, 0.0f, 0.0f};
	float  m_uiInputVel[3] = {0.0f, 0.0f, 0.0f};
	float  m_uiInputCol[3] = {1.0f, 1.0f, 1.0f};
	char   m_uiInputName[16] = {0};
};