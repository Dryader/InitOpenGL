#include "GameController.h"

#include "Fonts.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
    m_shaderColor = {};
    m_shaderDiffuse = {};
    m_meshBoxes.clear();
    m_meshLight = {};
    m_camera = {};
}

void GameController::Initialize()
{
    GLFWwindow* glfwWindow = WindowController::GetInstance().GetWindow();
    // Call this first, as it creates a window required by GLEW
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW."); // Initialize GLEW
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key
    glClearColor(0.1f, 0.1f, 0.01f, 0.0f); // black background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand(time(0)); // Seed random number generator

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
    m_shaderFont = Shader();
    m_shaderFont.LoadShaders("Shaders/Font.vertexshader.txt", "Shaders/Font.fragmentshader.txt");

    // Create meshes
    Mesh m = Mesh();
    m.Create(&m_shaderColor, "Assets/Models/Teapot.obj");
    m.SetPosition({1.0f, 0.0f, 0.0f});
    m.SetColor({1.0f, 1.0f, 1.0f});
    m.SetScale({0.01f, 0.01f, 0.01f});
    Mesh::Lights.push_back(m);

    Mesh teapot = Mesh();
    teapot.Create(&m_shaderDiffuse, "Assets/Models/Teapot.obj");
    teapot.SetCameraPosition(m_camera.GetPosition());
    teapot.SetScale({0.02f, 0.02f, 0.02f});
    teapot.SetPosition({0.0f, 0.0f, 0.0f});
    m_meshBoxes.push_back(teapot);

    Fonts f = Fonts();
    f.Create(&m_shaderFont, "arial.ttf", 100);

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
        for (unsigned int count = 0; count < m_meshBoxes.size(); count++)
        {
            m_meshBoxes[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }
        for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
        {
            Mesh::Lights[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }
        // Render three text strings in different colors and positions
        glDisable(GL_DEPTH_TEST);
        f.RenderText("Testing text", 10, 500, 0.5f, {1.0f, 1.0f, 0.0f}); // Yellow lower-left
        f.RenderText("Hello OpenGL", 10, 50, 0.4f, {0.0f, 1.0f, 0.0f});   // Green near top-left
        f.RenderText("Third line", 600, 300, 0.6f, {0.2f, 0.6f, 1.0f}); // Blue-ish mid-right
        glEnable(GL_DEPTH_TEST);

        glfwSwapBuffers(WindowController::GetInstance().GetWindow()); // Swap the back and front buffers
        glfwPollEvents();
    }
    while (glfwGetKey(WindowController::GetInstance().GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        // Check if the ESC key was pressed
        glfwWindowShouldClose(WindowController::GetInstance().GetWindow()) == 0); // Check if the window was closed

    for (int count = 0; count < Mesh::Lights.size(); count++)
    {
        Mesh::Lights[count].Cleanup();
    }
    for (unsigned int count = 0; count < m_meshBoxes.size(); count++)
    {
        m_meshBoxes[count].Cleanup();
    }
    m_shaderDiffuse.Cleanup();
    m_shaderColor.Cleanup();
}
