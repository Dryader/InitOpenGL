#include "GameController.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
    m_shaderColor = {};
    m_shaderDiffuse = {};
    m_meshBox = {};
    m_meshLight = {};
    m_camera = {};
}

void GameController::Initialize()
{
    GLFWwindow* glfwWindow = WindowController::GetInstance().GetWindow();
    // Call this first, as it creates a window required by GLEW
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW."); // Initialize GLEW
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black background
    glEnable(GL_DEPTH_TEST);

    //Create a default perspective camera
    m_camera = Camera(WindowController::GetInstance().GetResolution());
}

void GameController::RunGame()
{
    // Create and compile our GLSL programs from the shaders
    m_shaderColor = Shader();
    m_shaderColor.LoadShaders("Shaders/Color.vertexshader.txt", "Shaders/Color.fragmentshader.txt");
    m_shaderDiffuse = Shader();
    m_shaderDiffuse.LoadShaders("Shaders/Diffuse.vertexshader.txt", "Shaders/Diffuse.fragmentshader.txt");

    // Create meshes
    m_meshLight = Mesh();
    m_meshLight.Create(&m_shaderColor);
    m_meshLight.SetPosition({1, 0.5f, 0.5f});
    m_meshLight.SetScale({0.1f, 0.1f, 0.1f});

    m_meshBox = Mesh();
    m_meshBox.Create(&m_shaderDiffuse);
    m_meshBox.SetLightColor({0.5f, 0.9f, 0.5f});
    m_meshBox.SetLightPosition(m_meshLight.GetPosition());
    m_meshBox.SetCameraPosition(m_camera.GetPosition());

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
        m_meshBox.Render(m_camera.GetProjection() * m_camera.GetView());
        m_meshLight.Render(m_camera.GetProjection() * m_camera.GetView());
        glfwSwapBuffers(WindowController::GetInstance().GetWindow()); // Swap the back and front buffers
        glfwPollEvents();
    }
    while (glfwGetKey(WindowController::GetInstance().GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        // Check if the ESC key was pressed
        glfwWindowShouldClose(WindowController::GetInstance().GetWindow()) == 0); // Check if the window was closed

    m_meshLight.Cleanup();
    m_meshBox.Cleanup();
    m_shaderDiffuse.Cleanup();
    m_shaderColor.Cleanup();
}
