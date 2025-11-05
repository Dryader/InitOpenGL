#include "GameController.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
    m_shader = {};
    m_mesh = {};
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
    // Create and compile our GLSL program from the shaders
    m_shader = Shader();
    m_shader.LoadShaders("Shaders/Diffuse.vertexshader.txt", "Shaders/Diffuse.fragmentshader.txt");

    m_mesh = Mesh();
    m_mesh.Create(&m_shader);

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

        m_mesh.Render(m_camera.GetProjection() * m_camera.GetView());
        glfwSwapBuffers(WindowController::GetInstance().GetWindow()); // Swap the back and front buffers
        glfwPollEvents();
    }
    while (glfwGetKey(WindowController::GetInstance().GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS && // Check if the ESC key was pressed
           glfwWindowShouldClose(WindowController::GetInstance().GetWindow()) == 0); // Check if the window was closed

    m_mesh.Cleanup();
    m_shader.Cleanup();
}
