# InitOpenGL

A C++ OpenGL project demonstrating a basic game engine structure with various graphical features.

## Features

- **Shaders:** Support for multiple shader types (Vertex, Fragment, Color, Diffuse, Water, Skybox).
- **Skybox:** Implemented cubemap-based background.
- **Post-Processing:** Extensible post-processing system.
- **Mesh Loading:** Load models using OBJ_Loader.
- **Camera System:** First-person style camera control.
- **Font Rendering:** Text rendering support using FreeType.
- **Scenes:** Includes a water simulation scene and a space scene with a spaceship and asteroids.
- **Tool Window:** Integral WinForms tool window for interactive controls.

## Technologies Used

- **OpenGL:** Graphics API.
- **GLFW:** Windowing and input handling.
- **GLEW:** OpenGL extension loading.
- **GLM:** Mathematics library for graphics.
- **stb_image:** Image loading.
- **FreeType:** Font rendering.
- **OBJ_Loader:** Simple wavefront OBJ model loader.

## Getting Started

### Prerequisites

- Visual Studio (with C++/CLI support for the Tool Window).
- Windows OS.

### Building

1. Clone the repository.
2. Open `InitOpenGL.sln` in Visual Studio.
3. Build the solution (tested on x64 Debug/Release).
4. Run the project.

## Project Structure

- `OpenGL/`: Source code for the main application and engine components.
- `External/`: Third-party libraries and headers.
- `Shaders/`: GLSL shader files.
- `Assets/`: Models and textures.
