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

	// Active Main Shader
	m_mainShader.Activate();

	// Update View Matrix
	m_projection = glm::perspective(glm::radians(45.0f), (float)m_width / m_height, 0.1f, 300.0f);
	m_view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);

	// Update Projection matrix
	m_mainShader.SetUniformMatrix4fv("view", m_view);

	for (auto& planet : m_vPlanets) {
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
		Planet& planet = m_vPlanets.back();

		planet.renderer.SetPosition(planet.position);
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
	static float cameraSpeed = 5.0f * deltaTime; // Adjust based on frame time
	if (getKeyState(SDL_SCANCODE_W) == KEY_DOWN || getKeyState(SDL_SCANCODE_W) == KEY_HELD)
		m_cameraPos += cameraSpeed * m_cameraFront;
	if (getKeyState(SDL_SCANCODE_S) == KEY_DOWN || getKeyState(SDL_SCANCODE_S) == KEY_HELD)
		m_cameraPos -= cameraSpeed * m_cameraFront;
	if (getKeyState(SDL_SCANCODE_A) == KEY_DOWN || getKeyState(SDL_SCANCODE_A) == KEY_HELD)
		m_cameraPos -= glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
	if (getKeyState(SDL_SCANCODE_D) == KEY_DOWN || getKeyState(SDL_SCANCODE_D) == KEY_HELD)
		m_cameraPos += glm::normalize(glm::cross(m_cameraFront, m_cameraUp)) * cameraSpeed;
	if (getKeyState(SDL_SCANCODE_LSHIFT) == KEY_DOWN || getKeyState(SDL_SCANCODE_LSHIFT) == KEY_HELD)
		m_cameraPos -= cameraSpeed * m_cameraUp;
	if (getKeyState(SDL_SCANCODE_SPACE) == KEY_DOWN || getKeyState(SDL_SCANCODE_SPACE) == KEY_HELD)
		m_cameraPos += cameraSpeed * m_cameraUp;
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

