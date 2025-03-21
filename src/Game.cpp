#include "Game.h"

Game::Game(SDL_Window* window, SDL_GLContext glContext) 
	: m_pWindow(window), m_GLContext(glContext), m_mainShader(RESOURCES_PATH "vertex.vert", RESOURCES_PATH "fragment.frag"){

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

	// Get window width and height
	SDL_GetWindowSizeInPixels(m_pWindow, &m_width, &m_height);

	m_projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 300.0f); 
	m_view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

	// Update Projection matrix
	m_mainShader.SetUniformMatrix4fv("projection", m_projection);
	m_mainShader.SetUniformMatrix4fv("view", m_view);
}

void Game::PollEvents() {
	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL3_ProcessEvent(&event);
		switch (event.type) {
		case SDL_EVENT_QUIT:
			m_running = false;
			break;
		case SDL_EVENT_WINDOW_RESIZED:
			SDL_GetWindowSizeInPixels(m_pWindow, &m_width, &m_height);
			glViewport(0, 0, m_width, m_height);
			m_projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 300.0f);

			// Update Projection matrix
			m_mainShader.SetUniformMatrix4fv("projection", m_projection);
			break;
		case SDL_EVENT_KEY_UP:
			// Update current frame key press
			m_currentFrameKeyPress[event.key.scancode] = 0;
			break;
		case SDL_EVENT_KEY_DOWN:
			// Ignore repeat key press event
			if (event.key.repeat) break;

			// Update current frame key press
			m_currentFrameKeyPress[event.key.scancode] = 1;
			break;
		case SDL_EVENT_MOUSE_MOTION:
			handleMouseEvent(event);
			break;
		default:
			break;
		}
	}

}

bool Game::GameLoop() {
	// Handle Keyboard Input
	handleKeyboard();

	// Specify the color of the background
	glClearColor(0.035f, 0.075f, 0.085f, 1.0f);
	// Clean the back buffer and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	if(m_runSim) {
		// Apply Gravity
		size_t planetsCount = m_vPlanets.size();
		if (planetsCount > 1) {
			for (size_t i = 0; i < planetsCount - 1; ++i) {
				for (size_t j = i + 1; j < planetsCount; ++j) {
					ApplyGravity(m_vPlanets[i], m_vPlanets[j]);
				}
			}
		}
	}
	
	// Active Main Shader
	m_mainShader.Activate();

	// Update View Matrix
	m_projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 300.0f);
	m_view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

	// Update Projection matrix
	m_mainShader.SetUniformMatrix4fv("view", m_view);

	for (auto& planet : m_vPlanets) {
		if (m_runSim) {
			// Apply Movement to Planets Before Drawing only if sim is running
			planet.position += planet.velocity * float(deltaTime);
			planet.renderer.SetPosition(planet.position);
		}

		m_mainShader.SetUniformMatrix4fv("model", planet.renderer.GetModelMatrix());
		m_mainShader.SetUniform3fv("material", planet.material);
		planet.renderer.Draw();
	}

	// Copy current frame key press to last frame key press
	std::memcpy(m_lastFrameKeyPress, m_currentFrameKeyPress, sizeof(m_currentFrameKeyPress));

	return m_running;
}

void Game::RenderUI() {
	// Start the ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	// Main Menu
	ImGui::Begin("Main Menu");
	ImGui::Text("Welcome to the Gravity Simulation!");
	// Instead of placing raw text calls inside a ListBox, we can create an array of strings
	// and then use ImGui::Selectable for each item so that the ListBox works as intended.

	const char* simulationControls[] = {
		"Press TAB to toggle cursor mode",
		"Press ESC to exit the simulation",
		"Press SPACE to move up",
		"Press LSHIFT to move down",
		"Press W/A/S/D to move forward/left/backward/right"
	};

	if (ImGui::BeginListBox("Simulation Controls", ImVec2(0, IM_ARRAYSIZE(simulationControls) * ImGui::GetTextLineHeightWithSpacing())))
	{
		for (int i = 0; i < IM_ARRAYSIZE(simulationControls); i++)
		{
			// We pass 'false' for the selected flag since these are just informational
			ImGui::Selectable(simulationControls[i], false);
		}
		ImGui::EndListBox();
	}

	ImGui::Checkbox("Run Simulation", &m_runSim);
	ImGui::DragFloat("Simulation Speed", &m_timeMultiplier, 10.0f, 0.0f, 10000.0f);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Controls simulation speed.\nIncreasing this value speeds up the simulation but may reduce numerical accuracy.");
	ImGui::DragFloat("Camera Speed", &cameraSpeed, 1.0f, 1.0f, 100.0f);
	ImGui::DragFloat("Mouse Sensitivity", &m_cameraSensitivity, 1.0f, 1.0f, 100.0f);
	ImGui::End();

	// Add Planet Menu UI
	ImGui::Begin("Add Planet Menu");
	ImGui::Text("Add Planets to Scene!");
	ImGui::InputText("Enter Name for Planet:", m_uiInputName, 16);
	ImGui::InputDouble("Mass (in kg):", &m_uiInputMass);
	ImGui::InputDouble("Radius of Planet(in km):", &m_uiInputRadius);
	ImGui::InputFloat3("Position of Planet(in 10^3 km): ", m_uiInputPos);
	ImGui::InputFloat3("Velocity of Planet(in km/s): ", m_uiInputVel);
	ImGui::ColorEdit3("Color of Planet: ", m_uiInputCol);
	
	if (ImGui::Button("Add Planet")) {
		// Input Units:
		// MASS: m [kg]
		// RADIUS: r [km]
		// POSITION: P [10^3 km]
		// VELOCITY: V [km/s]

		// Conversion To Game Unit:
		// MASS: m [kg] -> m [kg] * kg2gm [gm = Game Mass Unit]
		// LENGTH: r [km] -> (r * 1000)[m] * meter2gl [gl = Game Length Unit]
		// TIME: [s] -> [s] * sec2gs [gs = Game Time Unit]
		// POSITION: P [10^3 km]-> (P * 10^3)[km] * kilometer2gl [gl]
		// VELOCITY: V [km/s] -> (V * 10^3 [m] * meter2gl) / ([s] * sec2gs) [gl/gs]
		
		// Reserve additional capacity if needed (without changing size)
		if (m_vPlanets.size() + 1 >= m_vPlanets.capacity()) {
			m_vPlanets.reserve(static_cast<size_t>(m_vPlanets.size() * 1.5f + 1.0f));
		}
		
		// Unit Conversions
		float convertedPos[3] = { m_uiInputPos[0] * 1000.0f * KM_TO_GLEN, m_uiInputPos[1] * 1000.0f * KM_TO_GLEN, m_uiInputPos[2] * 1000.0f * KM_TO_GLEN };
		float convertedVel[3] = { m_uiInputVel[0] * KM_TO_GLEN / SEC_TO_GSEC, m_uiInputVel[1] * KM_TO_GLEN / SEC_TO_GSEC, m_uiInputVel[2] * KM_TO_GLEN / SEC_TO_GSEC };

		m_vPlanets.emplace_back(m_uiInputMass * KG_TO_GMASS, m_uiInputRadius * KM_TO_GLEN, convertedPos, convertedVel, m_uiInputCol, m_uiInputName);
		Planet& planet = m_vPlanets.back();

		planet.renderer.SetPosition(planet.position);
	}
	ImGui::End();

	// TODO: Convert Displayed Units from Game Units to Units of interest
	ImGui::Begin("Plane Info");
	ImGui::Text("Planets in Scene: %d", m_vPlanets.size());
	ImGui::Text("Planet's Information: ");
	for (size_t i = 0; i < m_vPlanets.size(); ++i) {
		Planet& planet = m_vPlanets[i];
		ImGui::PushID(i);
		
		ImGui::Text("Planet Name(ID): %s(%d)", planet.name, i);
		ImGui::InputDouble("Planet Mass:", &planet.mass);
		if (ImGui::InputDouble("Planet Radius:", &planet.radius)) {
			// We need to update two planets one for the planet's radius and other for planet's renderer's radius
			// If the value has changed then update it
			planet.renderer.SetRadius(planet.radius);
		}
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

void Game::ApplyGravity(Planet& pl1, Planet& pl2) {
	glm::f64vec3 r12 = pl2.position - pl1.position; // Point from pl1 towards pl2

	if(r12.length() < 1e-10) return; // Prevent Division by Zero (Avoiding NaNs) (within when 0.1m range)
	
	double Force = (G * pl1.mass * pl2.mass) / (glm::dot(r12, r12)); // G / r^2
	r12 = glm::normalize(r12); // Normalize r12

	glm::f64vec3 accel1 = Force / pl1.mass * r12;
	glm::f64vec3 accel2 = Force / pl2.mass * -r12;
	
	// Now apply acceleration to the bodies
	pl1.velocity += accel1 * (deltaTime * m_timeMultiplier);
	pl2.velocity += accel2 * (deltaTime * m_timeMultiplier);
}

void Game::handleMouseEvent(SDL_Event& event) {
	if (!m_lookMode) return;

	// For absolute mouse motion
	//float x_pos = float(event.motion.x);
	//float y_pos = float(event.motion.y);

	//static float lastX = m_width / 2.0f, lastY = m_height / 2.0f;
	//static float yaw = -90.0f, pitch = 0.0f;
	//float xOffset = x_pos - lastX;
	//float yOffset = lastY - y_pos; // Inverted
	//lastX = x_pos;
	//lastY = y_pos;

	// For relative mouse motion
	static float yaw = -90.0f, pitch = 0.0f;
	float xOffset = float(event.motion.xrel);
	float yOffset = -float(event.motion.yrel); // Inverted

	float sensitivity = m_cameraSensitivity * deltaTime;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	yaw += xOffset;
	pitch += yOffset;
	pitch = glm::clamp(pitch, -89.0f, 89.0f); // Prevent flipping

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_cameraFront = glm::normalize(direction);
}

void Game::handleKeyboard() {

	// TAB KEY PRESS
	if (getKeyState(SDL_SCANCODE_TAB) == KEY_DOWN) {
		// Toggle Cursor
		m_lookMode = !m_lookMode;
		SDL_SetWindowRelativeMouseMode(m_pWindow, m_lookMode);
		m_lookMode ? SDL_HideCursor() : SDL_ShowCursor();
	}

	// ESC KEY PRESS
	if (getKeyState(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		// Toggle running state
		m_running = false;
	}

	// Update Camera Position
	// Adjust based on frame time
	if (getKeyState(SDL_SCANCODE_W) == KEY_DOWN || getKeyState(SDL_SCANCODE_W) == KEY_HELD)
		m_cameraPos += float(cameraSpeed * deltaTime) * m_cameraFront;
	if (getKeyState(SDL_SCANCODE_S) == KEY_DOWN || getKeyState(SDL_SCANCODE_S) == KEY_HELD)
		m_cameraPos -= float(cameraSpeed * deltaTime) * m_cameraFront;
	if (getKeyState(SDL_SCANCODE_A) == KEY_DOWN || getKeyState(SDL_SCANCODE_A) == KEY_HELD)
		m_cameraPos -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * float(cameraSpeed * deltaTime);
	if (getKeyState(SDL_SCANCODE_D) == KEY_DOWN || getKeyState(SDL_SCANCODE_D) == KEY_HELD)
		m_cameraPos += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * float(cameraSpeed * deltaTime);
	if (getKeyState(SDL_SCANCODE_LSHIFT) == KEY_DOWN || getKeyState(SDL_SCANCODE_LSHIFT) == KEY_HELD)
		m_cameraPos -= float(cameraSpeed * deltaTime) * m_cameraUp;
	if (getKeyState(SDL_SCANCODE_SPACE) == KEY_DOWN || getKeyState(SDL_SCANCODE_SPACE) == KEY_HELD)
		m_cameraPos += float(deltaTime * cameraSpeed) * m_cameraUp;
}

Game::KeyState Game::getKeyState(SDL_Scancode scancode) {

	if (m_currentFrameKeyPress[scancode]) {
		if (m_lastFrameKeyPress[scancode]) return KEY_HELD;
		else return KEY_DOWN;
	}
	else {
		if (m_lastFrameKeyPress[scancode]) return KEY_UP;
		else return KEY_NOT_PRESSED;
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

