#include "GameController.h"

#include "Fonts.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
    m_shaderColor = {};
    m_shaderDiffuse = {};
    m_meshes.clear();
    m_camera = {};
}

void GameController::Initialize()
{
    GLFWwindow* glfwWindow = WindowController::GetInstance().GetWindow();
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW."); // Initialize GLEW
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f); // gray background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    srand(static_cast<unsigned int>(time(0))); // Seed random number generator
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

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
    auto m = Mesh();
    m.Create(&m_shaderColor, "Assets/Models/Teapot.obj");
    m.SetPosition({0.0f, 0.8f, 1.0f});
    m.SetColor({1.0f, 1.0f, 1.0f});
    m.SetScale({0.01f, 0.01f, 0.01f});
    m.SetCameraPosition(m_camera.GetPosition());
    Mesh::Lights.push_back(m);

    auto cube = Mesh();
    cube.Create(&m_shaderDiffuse, "Assets/Models/Cube.obj", 1000);
    cube.SetCameraPosition(m_camera.GetPosition());
    cube.SetScale({0.05f, 0.05f, 0.05f});
    cube.SetPosition({0.0f, 0.0f, 0.0f});
    m_meshes.push_back(cube);

    auto f = Fonts();
    f.Create(&m_shaderFont, "arial.ttf", 40);

    double lastTime = glfwGetTime();
    int fps = 0;
    string fpsS = "0";

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0)
        {
            fpsS = "FPS: " + to_string(fps);
            fps = 0;
            lastTime += 1.0;
        }
        f.RenderText(fpsS, 100, 100, 0.5f, {1.0f, 1.0f, 0.0f});

        for (unsigned int count = 0; count < m_meshes.size(); count++)
        {
            m_meshes[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }
        for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
        {
            Mesh::Lights[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }

        glfwSwapBuffers(WindowController::GetInstance().GetWindow()); // Swap the back and front buffers
        glfwPollEvents();
    }
    while (glfwGetKey(WindowController::GetInstance().GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(WindowController::GetInstance().GetWindow()) == 0);

    for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
    {
        Mesh::Lights[count].Cleanup();
    }
    for (unsigned int count = 0; count < m_meshes.size(); count++)
    {
        m_meshes[count].Cleanup();
    }
    // for (unsigned int count = 0; count < m_meshes.size(); count++)
    // {
    //     for (int x = 0; x < 1000; x++)
    //     {
    //         m_meshes[count].Render(m_camera.GetProjection() * m_camera.GetView());
    //     }
    // }

    m_skybox.Cleanup();
    m_shaderDiffuse.Cleanup();
    m_shaderColor.Cleanup();
    m_shaderSkybox.Cleanup();
}
