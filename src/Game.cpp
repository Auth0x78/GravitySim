#include "Game.h"

Game::Game(SDL_Window* window, SDL_GLContext glContext) 
	: m_pWindow(window), m_GLContext(glContext) {
	// ImGui Initialisation
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	m_pIO = &ImGui::GetIO();
	m_pIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
	m_pIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable docking
	m_pIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable multi-viewport (multi-window support)
	m_pIO->ConfigViewportsNoTaskBarIcon = true;

	ImGui_ImplSDL3_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 440");
}

void Game::GameLoop() {
	// Specify the color of the background
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}

void Game::RenderUI() {
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Add Planet Menu UI
	ImGui::Begin("Add Planet Menu");
	ImGui::Text("This is for adding planets to scene!");
	ImGui::InputText("Enter Name for Planet:", m_uiInputName, 16);
	ImGui::InputDouble("Mass (in kg):", &m_uiInputMass);
	ImGui::InputDouble("Radius of Planet(in km):", &m_uiInputRadius);
	ImGui::InputFloat3("Position of Planet(in 10^3km): ", m_uiInputPos);
	ImGui::InputFloat3("Velocity of Planet(in km/s): ", m_uiInputVel);
	ImGui::ColorEdit3("Color of Planet: ", m_uiInputCol);
	
	if (ImGui::Button("Add Planet")) {
		// TODO: Convert Input-ed Units to Game Units
		// For now assume, game units are same as input units
		// Add Planet to Scene
		
		// Reserve additional capacity if needed (without changing size)
		if (m_vPlanets.size() + 1 >= m_vPlanets.capacity()) {
			m_vPlanets.reserve(static_cast<size_t>(m_vPlanets.size() * 1.5f + 1.0f));
		}

		m_vPlanets.emplace_back(m_uiInputMass, m_uiInputRadius, m_uiInputPos, m_uiInputVel, m_uiInputCol, m_uiInputName);
	}
	ImGui::End();

	ImGui::Begin("Plane Info");
	ImGui::Text("Planets in Scene: %d", m_vPlanets.size());
	ImGui::Text("Planet's Information: ");
	for (size_t i = 0; i < m_vPlanets.size(); ++i) {
		Planet& planet = m_vPlanets[i];
		ImGui::PushID(i);
		
		ImGui::Text("Planet Name(ID): %s(%d)", planet.name, i);
		ImGui::InputDouble("Planet Mass:", &planet.mass);
		ImGui::InputDouble("Planet Radius:", &planet.radius);
		ImGui::Text("Planet Position: (%f, %f, %f)", planet.position.x, planet.position.y, planet.position.z);
		ImGui::Text("Planet Velocity: (%f, %f, %f)", planet.velocity.x, planet.velocity.y, planet.velocity.z);
		ImGui::Text("Planet Material: (%f, %f, %f)", planet.material.x, planet.material.y, planet.material.z);

		ImGui::PopID();
	}
	ImGui::End();

	// Render ImGui
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Handle multiple viewports
	if (m_pIO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		SDL_Window* backup_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_window, backup_context);
	}
}

Game::~Game() {
	Shutdown();
}

void Game::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
}
