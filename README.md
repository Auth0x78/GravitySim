# GravitySim

GravitySim is a real-time gravity simulation built with SDL3 and OpenGL. This project demonstrates interactive physics, rendering techniques, and an ImGui-powered user interface with docking support.

## Features

- **Real-Time Simulation:** Experience accurate gravity-based motion in real time.
- **SDL3 & OpenGL Rendering:** Leverages modern graphics with SDL3.
- **ImGui Docking:** Integrated ImGui UI with docking and multi-viewport support.
- **Cross-Platform:** Designed to run on multiple operating systems.
- **Customizable Controls:** Supports keyboard and mouse input for interactive simulation.

## Installation

### Prerequisites

- [SDL3](https://libsdl.org/)
- [OpenGL](https://www.opengl.org/)
- [ImGui](https://github.com/ocornut/imgui) along with its SDL3 and OpenGL backends
- A C++17 compatible compiler

### Build Instructions

1. **Clone the Repository**
   ```bash
   git clone https://github.com/Auth0x78/GravitySim.git
   cd GravitySim
   ```

2. **Create a Build Directory and Configure the Project**
   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. **Build the Project**
   ```bash
   cmake --build .
   ```

4. **Run GravitySim**
   ```bash
   ./GravitySim
   ```

## Usage

- **Movement:** Use **W/A/S/D** to move around.
- **Mouse Interaction:** Use the mouse to control camera or UI interactions.
- **UI Controls:** The integrated ImGui interface provides additional simulation controls and settings.

## Screenshots

*Include screenshots or GIFs here to showcase GravitySim in action.*

![GravitySim Screenshot](![image](https://github.com/user-attachments/assets/d2506491-185c-4978-b339-15e80c30729c)
)

## Contributing

Contributions are welcome! If you would like to contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/YourFeature`).
3. Commit your changes (`git commit -am 'Add new feature'`).
4. Push the branch (`git push origin feature/YourFeature`).
5. Open a Pull Request.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgements

- [SDL3](https://libsdl.org/) for providing a powerful multimedia library.
- [ImGui](https://github.com/ocornut/imgui) for the immediate-mode GUI.
- All contributors and users of GravitySim.
