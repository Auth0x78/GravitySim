#pragma once
#pragma region Main Headers
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <openglDebug.h>
#include <vector>
#include <iostream>
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
	void GameLoop();
	void RenderUI();

	~Game();
	void Shutdown();

private:
	SDL_Window* m_pWindow;
	SDL_GLContext m_GLContext;
	ImGuiIO* m_pIO;

	// Game Variables
	std::vector<Planet> m_vPlanets;

	// Add Planet Menu Variables
	double m_uiInputMass = 0;
	double m_uiInputRadius = 0;
	float  m_uiInputPos[3] = {0.0f, 0.0f, 0.0f};
	float  m_uiInputVel[3] = {0.0f, 0.0f, 0.0f};
	float  m_uiInputCol[3] = {0.0f, 0.0f, 0.0f};
	char   m_uiInputName[16] = {0};
};