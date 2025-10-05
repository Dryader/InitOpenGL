#include "GameController.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
	m_shader = { };
    m_mesh = { };
	m_camera = { };
}

void GameController::Initialize()
{
    GLFWwindow* glfwWindow = WindowController::GetInstance().GetWindow(); // Call this first, as it creates a window required by GLEW
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW."); // Initialize GLEW
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // black background
    glEnable(GL_CULL_FACE);
	//Create a default perspective camera
	m_camera = Camera(WindowController::GetInstance().GetResolution());
}

void GameController::RunGame()
{
    m_shader = Shader();
    m_shader.LoadShaders("Shaders/SimpleVertexShader.vertexshader.txt", "Shaders/SimpleFragmentShader.fragmentshader.txt");
    m_mesh = Mesh();
    m_mesh.Create(&m_shader);

    GLFWwindow* window = WindowController::GetInstance().GetWindow();

    glUseProgram(m_shader.GetProgramID());
    // Samplers set in Mesh::Render, but we initialize mode/blendFactor here
    int mode = 0;             // 0 = single texture * color
    float blendFactor = 0.3f; // initial mix for modes 1 / 2
    glUniform1i(m_shader.GetUniformMode(), mode);
    glUniform1f(m_shader.GetUniformBlendFactor(), blendFactor);

    do
    {
        // Input handling for mode switching
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) mode = 0;               // single texture * color
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) mode = 1;          // two textures
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) mode = 2;          // two textures * color

        // Adjust blend factor (only meaningful for modes 1 & 2) with Up/Down
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)   blendFactor += 0.01f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) blendFactor -= 0.01f;
        if (blendFactor > 1.0f) blendFactor = 1.0f;
        if (blendFactor < 0.0f) blendFactor = 0.0f;

        glUseProgram(m_shader.GetProgramID());
        glUniform1i(m_shader.GetUniformMode(), mode);
        glUniform1f(m_shader.GetUniformBlendFactor(), blendFactor);

        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
        m_mesh.Render(m_camera.GetProjection()* m_camera.GetView());
        glfwSwapBuffers(window); // Swap the front and back buffers
        glfwPollEvents();
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS );

    m_mesh.Cleanup();
    m_shader.Cleanup();
}
