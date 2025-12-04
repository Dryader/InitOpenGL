#include "GameController.h"

#include "Fonts.h"
#include "WindowController.h"
#include "ToolWindow.h"

GameController::GameController()
{
    m_shaderColor = {};
    m_shaderDiffuse = {};
    m_shaderFont = {};
    m_camera = {};
    m_shaderPost = {};
    m_meshes.clear();
    vao = 0;
}

void GameController::Initialize()
{
    GLFWwindow* glfwWindow = WindowController::GetInstance().GetWindow();
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW."); // Initialize GLEW
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE); // Ensure we can capture the escape key
    glClearColor(0, 0, 0, 1); // Black background
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    srand(static_cast<unsigned int>(time(0))); // Seed random number generator
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Create a default perspective camera
    Resolution r = WindowController::GetInstance().GetResolution();
    glViewport(0, 0, r.m_width, r.m_height);
    m_camera = Camera(r);
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

    m_shaderPost = Shader();
    m_shaderPost.LoadShaders("Shaders/PostProcessor.vertexshader.txt", "Shaders/PostProcessor.fragmentshader.txt");

    // Create meshes

    auto m = Mesh();
    m.Create(&m_shaderColor, "Assets/Models/Sphere.obj");
    m.SetPosition(glm::vec3(0.0f, 0.5f, -0.5f));
    m.SetColor({1.0f, 1.0f, 1.0f});
    m.SetScale({0.001f, 0.001f, 0.001f});
    Mesh::Lights.push_back(m);

    Mesh fighter = Mesh();
    fighter.Create(&m_shaderDiffuse, "Assets/Models/Fighter.obj");
    fighter.SetCameraPosition(m_camera.GetPosition());
    fighter.SetPosition({0.0f, 0.0f, 0.0f});
    fighter.SetRotation({0.0f, 0.0f, 0.0f});
    fighter.SetScale({0.0008f, 0.0008f, 0.0008f});
    m_meshes.push_back(fighter);


    auto f = Fonts();
    f.Create(&m_shaderFont, "arial.ttf", 40);
    m_postProcessor = PostProcessor();
    m_postProcessor.Create(&m_shaderPost);

    double lastTime = glfwGetTime();
    int fps = 0;
    string fpsS = "0";
    GLFWwindow* window = WindowController::GetInstance().GetWindow();

    // Mouse handling variables
    static double lastMouseX = 0, lastMouseY = 0;
    static bool leftButtonWasPressed = false;

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen
        m_postProcessor.Start();

        // Handle mouse interactions for fighter transformations
        if (m_meshes.size() > 0)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

            // Auto-rotate fighter when not in Transform mode
            if (!OpenGL::ToolWindow::TransformEnabled)
            {
                glm::vec3 currentRot = m_meshes[0].GetRotation();
                m_meshes[0].SetRotation(glm::vec3(currentRot.x + 0.04f, currentRot.y, currentRot.z));
            }

            if (leftButtonState == GLFW_PRESS)
            {
                if (!leftButtonWasPressed)
                {
                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                    leftButtonWasPressed = true;
                }
                else
                {
                    double deltaX = mouseX - lastMouseX;
                    double deltaY = mouseY - lastMouseY;

                    // Apply transformations based on ToolWindow settings
                    if (OpenGL::ToolWindow::TranslateEnabled)
                    {
                        glm::vec3 currentPos = m_meshes[0].GetPosition();
                        m_meshes[0].SetPosition(glm::vec3(currentPos.x + deltaX * 0.001f,
                                                          currentPos.y - deltaY * 0.001f,
                                                          currentPos.z));
                    }

                    if (OpenGL::ToolWindow::RotateEnabled)
                    {
                        glm::vec3 currentRot = m_meshes[0].GetRotation();
                        m_meshes[0].SetRotation(glm::vec3(currentRot.x + deltaY * 0.5f,
                                                          currentRot.y + deltaX * 0.5f,
                                                          currentRot.z));
                    }

                    if (OpenGL::ToolWindow::ScaleEnabled)
                    {
                        glm::vec3 currentScale = m_meshes[0].GetScale();
                        // Horizontal movement affects X scale (left/right stretch)
                        float scaleFactorX = 1.0f + (deltaX * 0.005f);
                        // Vertical movement affects Y scale (up/down stretch)
                        float scaleFactorY = 1.0f + (deltaY * 0.005f);
                        m_meshes[0].SetScale(glm::vec3(currentScale.x * scaleFactorX,
                                                       currentScale.y * scaleFactorY,
                                                       currentScale.z));
                    }

                    lastMouseX = mouseX;
                    lastMouseY = mouseY;
                }
            }
            else
            {
                leftButtonWasPressed = false;
            }

        // Handle light position reset
            if (OpenGL::ToolWindow::ResetLightPositionRequested && Mesh::Lights.size() > 0)
            {
                Mesh::Lights[0].SetPosition(glm::vec3(OpenGL::ToolWindow::DefaultLightPositionX, 
                                                       OpenGL::ToolWindow::DefaultLightPositionY, 
                                                       OpenGL::ToolWindow::DefaultLightPositionZ));
                OpenGL::ToolWindow::ResetLightPositionRequested = false;
            }

            // Handle light movement - only when left click is down and MoveLightEnabled is true
            if (OpenGL::ToolWindow::MoveLightEnabled && Mesh::Lights.size() > 0 && leftButtonState == GLFW_PRESS)
            {
                // Convert mouse coordinates to world space (rough approximation)
                // Mouse X: -1 to 1 mapped to screen width
                // Mouse Y: -1 to 1 mapped to screen height
                Resolution r = WindowController::GetInstance().GetResolution();
                float normalizedX = (static_cast<float>(mouseX) / r.m_width) * 2.0f - 1.0f;
                float normalizedY = 1.0f - (static_cast<float>(mouseY) / r.m_height) * 2.0f;

                // Update light position to follow mouse, keeping it at a fixed distance from camera
                Mesh::Lights[0].SetPosition(glm::vec3(normalizedX * 0.5f, 0.5f + normalizedY * 0.5f, 1.0f));
            }
        }

        for (unsigned int count = 0; count < m_meshes.size(); count++)
        {
            m_meshes[count].SetCameraPosition(m_camera.GetPosition());
            m_meshes[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }
        for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
        {
            Mesh::Lights[count].Render(m_camera.GetProjection() * m_camera.GetView());
        }

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0)
        {
            fpsS = "FPS: " + to_string(fps);
            fps = 0;
            lastTime = currentTime;
        }
        m_postProcessor.End();

        // Render FPS
        f.RenderText(fpsS, 100, 100, 0.5f, {1.0f, 1.0f, 0.0f});

        // Get and render mouse position
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        char mousePosBuffer[128];
        sprintf_s(mousePosBuffer, "Mouse Pos  : %.6f   %.6f", mouseX, mouseY);
        string mousePosStr = mousePosBuffer;
        f.RenderText(mousePosStr, 100, 130, 0.5f, {1.0f, 1.0f, 0.0f});

        // Get and render mouse button status
        int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int middleButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
        string leftButtonStr = (leftButtonState == GLFW_PRESS) ? "Left Button: Down" : "Left Button: Up";
        string middleButtonStr = (middleButtonState == GLFW_PRESS) ? "Middle Button: Down" : "Middle Button: Up";
        f.RenderText(leftButtonStr, 100, 160, 0.5f, {1.0f, 1.0f, 0.0f});
        f.RenderText(middleButtonStr, 100, 190, 0.5f, {1.0f, 1.0f, 0.0f});

        // Render fighter information if meshes exist
        if (m_meshes.size() > 0)
        {
            glm::vec3 fighterPos = m_meshes[0].GetPosition();
            glm::vec3 fighterRot = m_meshes[0].GetRotation();
            glm::vec3 fighterScale = m_meshes[0].GetScale();

            char posBuffer[256];
            char rotBuffer[256];
            char scaleBuffer[256];

            sprintf_s(posBuffer, "Fighter Position: (vec3(%.6f, %.6f, %.6f))", fighterPos.x, fighterPos.y,
                      fighterPos.z);
            sprintf_s(rotBuffer, "Fighter Rotation: (vec3(%.6f, %.6f, %.6f))", fighterRot.x, fighterRot.y,
                      fighterRot.z);
            sprintf_s(scaleBuffer, "Fighter Scale  : (vec3(%.6f, %.6f, %.6f))", fighterScale.x, fighterScale.y,
                      fighterScale.z);

            f.RenderText(string(posBuffer), 100, 220, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(rotBuffer), 100, 250, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(scaleBuffer), 100, 280, 0.5f, {1.0f, 1.0f, 0.0f});
        }

        glfwSwapBuffers(window); // Swap the back and front buffers
        glfwPollEvents();
    }
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);

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

    f.Cleanup();
    m_postProcessor.Cleanup();
    m_skybox.Cleanup();
    m_shaderDiffuse.Cleanup();
    m_shaderColor.Cleanup();
    m_shaderFont.Cleanup();
    m_shaderPost.Cleanup();
}
