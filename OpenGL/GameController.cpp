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
    m_shaderWater = {};
    m_meshes.clear();
    vao = 0;
    m_gameTime = 0.0;
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

    m_shaderWater = Shader();
    m_shaderWater.LoadShaders("Shaders/Water.vertexshader.txt", "Shaders/Water.fragmentshader.txt");

    m_shaderSkybox = Shader();
    m_shaderSkybox.LoadShaders("Shaders/Skybox.vertexshader.txt", "Shaders/Skybox.fragmentshader.txt");

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

    // Water scene setup - Fish with Atlantic bluefin tuna textures
    m_waterPlane = Mesh();
    m_waterPlane.Create(&m_shaderWater, "Assets/Models/Fish.obj");
    m_waterPlane.SetCameraPosition(m_camera.GetPosition());
    m_waterPlane.SetPosition({0.0f, 0.0f, 0.0f});
    m_waterPlane.SetRotation({0.0f, 180.0f, 0.0f});
    m_waterPlane.SetScale({0.020000f, 0.020000f, 0.020000f});

    // Skybox setup
    vector<string> faces = {
        "Assets/Textures/right.jpg",    // Right
        "Assets/Textures/left.jpg",     // Left
        "Assets/Textures/top.jpg",      // Top
        "Assets/Textures/bottom.jpg",   // Bottom
        "Assets/Textures/front.jpg",    // Front
        "Assets/Textures/back.jpg"      // Back
    };
    m_skybox.Create(&m_shaderSkybox, "Assets/Models/SkyBox.obj", faces);

    // Space scene setup - Spaceship at origin facing away from camera
    m_spaceship = Mesh();
    m_spaceship.Create(&m_shaderDiffuse, "Assets/Models/Fighter.obj");
    m_spaceship.SetCameraPosition(m_camera.GetPosition());
    m_spaceship.SetPosition({0.0f, 0.0f, 0.0f});
    m_spaceship.SetRotation({0.0f, 180.0f, 0.0f});  // Face away from camera
    m_spaceship.SetScale({0.0008f, 0.0008f, 0.0008f});

    // Asteroids - instance rendered around spaceship (100 asteroids)
    m_asteroids = Mesh();
    m_asteroids.Create(&m_shaderDiffuse, "Assets/Models/asteroid.obj", 100);
    m_asteroids.SetCameraPosition(m_camera.GetPosition());


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
        
        // Use post-processor for water scene wave effect only
        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            m_postProcessor.Start();
        }

        // Handle mouse interactions for fighter transformations
        if (m_meshes.size() > 0)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

            // Auto-rotate fighter when not in Transform mode
            if (!OpenGL::ToolWindow::TransformEnabled && !OpenGL::ToolWindow::WaterSceneEnabled)
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
                    if (OpenGL::ToolWindow::TranslateEnabled && !OpenGL::ToolWindow::WaterSceneEnabled)
                    {
                        glm::vec3 currentPos = m_meshes[0].GetPosition();
                        m_meshes[0].SetPosition(glm::vec3(currentPos.x + deltaX * 0.001f,
                                                          currentPos.y - deltaY * 0.001f,
                                                          currentPos.z));
                    }

                    if (OpenGL::ToolWindow::RotateEnabled && !OpenGL::ToolWindow::WaterSceneEnabled)
                    {
                        glm::vec3 currentRot = m_meshes[0].GetRotation();
                        m_meshes[0].SetRotation(glm::vec3(currentRot.x + deltaY * 0.5f,
                                                          currentRot.y + deltaX * 0.5f,
                                                          currentRot.z));
                    }

                    if (OpenGL::ToolWindow::ScaleEnabled && !OpenGL::ToolWindow::WaterSceneEnabled)
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

        // Update game time
        m_gameTime += 0.01;

        // Rotate camera in space scene
        if (OpenGL::ToolWindow::SpaceSceneEnabled)
        {
            m_camera.Rotate();
        }

        // Render appropriate scene based on ToolWindow selection
        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            // Render water scene
            m_waterPlane.SetCameraPosition(m_camera.GetPosition());
            glUseProgram(m_shaderWater.GetProgramID());
            m_shaderWater.SetFloat("Time", static_cast<float>(m_gameTime));
            m_shaderWater.SetFloat("Frequency", OpenGL::ToolWindow::WaterScaleFrequency);
            m_shaderWater.SetFloat("Amplitude", OpenGL::ToolWindow::WaterScaleAmplitude);
            glUseProgram(0);
            m_waterPlane.Render(m_camera.GetProjection() * m_camera.GetView());
        }
        else if (OpenGL::ToolWindow::SpaceSceneEnabled)
        {
            // Render space scene with skybox
            m_skybox.Render(m_camera.GetProjection() * m_camera.GetView());

            // Render spaceship
            m_spaceship.SetCameraPosition(m_camera.GetPosition());
            m_spaceship.Render(m_camera.GetProjection() * m_camera.GetView());

            // Render instance-rendered asteroids
            m_asteroids.SetCameraPosition(m_camera.GetPosition());
            m_asteroids.Render(m_camera.GetProjection() * m_camera.GetView());
        }
        else
        {
            // Render default fighter scene
            for (unsigned int count = 0; count < m_meshes.size(); count++)
            {
                m_meshes[count].SetCameraPosition(m_camera.GetPosition());
                m_meshes[count].Render(m_camera.GetProjection() * m_camera.GetView());
            }
        }

        // Only render lights for fighter scene, not water or space scene
        if (!OpenGL::ToolWindow::WaterSceneEnabled && !OpenGL::ToolWindow::SpaceSceneEnabled)
        {
            for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
            {
                Mesh::Lights[count].Render(m_camera.GetProjection() * m_camera.GetView());
            }
        }

        // Apply wireframe rendering if enabled
        if (OpenGL::ToolWindow::WireframeRenderEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Update post-processor tint blue uniform for both scenes
        glUseProgram(m_shaderPost.GetProgramID());
        m_shaderPost.SetBool("TintBlueEnabled", OpenGL::ToolWindow::TintBlueEnabled);
        glUseProgram(0);

        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - lastTime >= 1.0)
        {
            fpsS = "FPS: " + to_string(fps);
            fps = 0;
            lastTime = currentTime;
        }
        
        // Render FPS and other text BEFORE post-processor ends, so they get affected by wave effect
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

        // Render fighter/fish information based on scene
        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            // Render fish information for water scene
            glm::vec3 fishPos = m_waterPlane.GetPosition();
            glm::vec3 fishRot = m_waterPlane.GetRotation();
            glm::vec3 fishScale = m_waterPlane.GetScale();

            char posBuffer[256];
            char rotBuffer[256];
            char scaleBuffer[256];

            sprintf_s(posBuffer, "Fish Position: (vec3(%.6f, %.6f, %.6f))", fishPos.x, fishPos.y,
                      fishPos.z);
            sprintf_s(rotBuffer, "Fish Rotation: (vec3(%.6f, %.6f, %.6f))", fishRot.x, fishRot.y,
                      fishRot.z);
            sprintf_s(scaleBuffer, "Fish Scale  : (vec3(%.6f, %.6f, %.6f))", fishScale.x, fishScale.y,
                      fishScale.z);

            f.RenderText(string(posBuffer), 100, 220, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(rotBuffer), 100, 250, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(scaleBuffer), 100, 280, 0.5f, {1.0f, 1.0f, 0.0f});
        }
        else if (m_meshes.size() > 0)
        {
            // Render fighter information for fighter scene
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
        
        // End post-processor only if it was started (for water scene)
        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            // For water scene, set the water effect uniforms before ending post-processor
            glUseProgram(m_shaderPost.GetProgramID());
            m_shaderPost.SetFloat("Time", static_cast<float>(m_gameTime));
            m_shaderPost.SetFloat("Frequency", OpenGL::ToolWindow::WaterScaleFrequency);
            m_shaderPost.SetFloat("Amplitude", OpenGL::ToolWindow::WaterScaleAmplitude);
            glUseProgram(0);
            m_postProcessor.End();
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
