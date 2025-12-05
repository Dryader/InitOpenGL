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
    M_ASSERT(glewInit() == GLEW_OK, "Failed to initialize GLEW.");
    glfwSetInputMode(glfwWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    srand(static_cast<unsigned int>(time(0)));
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    Resolution r = WindowController::GetInstance().GetResolution();
    glViewport(0, 0, r.m_width, r.m_height);
    m_camera = Camera(r);
}

void GameController::RunGame()
{
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

    m_waterPlane = Mesh();
    m_waterPlane.Create(&m_shaderWater, "Assets/Models/Fish.obj");
    m_waterPlane.SetCameraPosition(m_camera.GetPosition());
    m_waterPlane.SetPosition({0.0f, 0.0f, 0.0f});
    m_waterPlane.SetRotation({0.0f, 180.0f, 0.0f});
    m_waterPlane.SetScale({0.020000f, 0.020000f, 0.020000f});

    vector<string> faces = {
        "Assets/Textures/right.jpg",
        "Assets/Textures/left.jpg",
        "Assets/Textures/top.jpg",
        "Assets/Textures/bottom.jpg",
        "Assets/Textures/back.jpg",
        "Assets/Textures/front.jpg"
    };
    m_skybox.Create(&m_shaderSkybox, "Assets/Models/SkyBox.obj", faces);

    m_spaceship = Mesh();
    m_spaceship.Create(&m_shaderDiffuse, "Assets/Models/Fighter.obj");
    m_spaceship.SetCameraPosition(m_camera.GetPosition());
    m_spaceship.SetPosition({0.0f, 0.0f, 0.0f});
    m_spaceship.SetRotation({0.0f, 180.0f, 0.0f});
    m_spaceship.SetScale({0.0008f, 0.0008f, 0.0008f});

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

    static double lastMouseX = 0, lastMouseY = 0;
    static bool leftButtonWasPressed = false;

    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            m_postProcessor.Start();
        }

        if (m_meshes.size() > 0)
        {
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

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
                        float scaleFactorX = 1.0f + (deltaX * 0.005f);
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

            if (OpenGL::ToolWindow::ResetLightPositionRequested && Mesh::Lights.size() > 0)
            {
                Mesh::Lights[0].SetPosition(glm::vec3(OpenGL::ToolWindow::DefaultLightPositionX,
                                                      OpenGL::ToolWindow::DefaultLightPositionY,
                                                      OpenGL::ToolWindow::DefaultLightPositionZ));
                OpenGL::ToolWindow::ResetLightPositionRequested = false;
            }

            if (OpenGL::ToolWindow::ResetTransformRequested && m_meshes.size() > 0)
            {
                m_meshes[0].SetPosition({0.0f, 0.0f, 0.0f});
                m_meshes[0].SetRotation({0.0f, 0.0f, 0.0f});
                m_meshes[0].SetScale({0.0008f, 0.0008f, 0.0008f});
                OpenGL::ToolWindow::ResetTransformRequested = false;
            }

            if (OpenGL::ToolWindow::MoveLightEnabled && Mesh::Lights.size() > 0 && leftButtonState == GLFW_PRESS)
            {
                Resolution r = WindowController::GetInstance().GetResolution();
                float normalizedX = (static_cast<float>(mouseX) / r.m_width) * 2.0f - 1.0f;
                float normalizedY = 1.0f - (static_cast<float>(mouseY) / r.m_height) * 2.0f;

                Mesh::Lights[0].SetPosition(glm::vec3(normalizedX * 1.5f, normalizedY * 1.5f, 1.0f));
            }
        }

        m_gameTime += 0.01;

        bool currentWaterSceneEnabled = OpenGL::ToolWindow::WaterSceneEnabled;
        bool currentSpaceSceneEnabled = OpenGL::ToolWindow::SpaceSceneEnabled;

        if (currentWaterSceneEnabled != m_previousWaterSceneEnabled ||
            currentSpaceSceneEnabled != m_previousSpaceSceneEnabled)
        {
            m_camera.ResetRotation();
            m_previousWaterSceneEnabled = currentWaterSceneEnabled;
            m_previousSpaceSceneEnabled = currentSpaceSceneEnabled;
        }

        if (OpenGL::ToolWindow::SpaceSceneEnabled)
        {
            m_camera.Rotate();
        }

        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
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
            m_skybox.Render(m_camera.GetProjection() * m_camera.GetView());

            m_spaceship.SetCameraPosition(m_camera.GetPosition());
            m_spaceship.Render(m_camera.GetProjection() * m_camera.GetView());

            m_asteroids.UpdateInstanceRotations(0.016f);
            m_asteroids.SetCameraPosition(m_camera.GetPosition());
            m_asteroids.Render(m_camera.GetProjection() * m_camera.GetView());
        }
        else
        {
            if (m_meshes.size() > 0 && !OpenGL::ToolWindow::WaterSceneEnabled && !OpenGL::ToolWindow::SpaceSceneEnabled)
            {
                m_meshes[0].SetCameraPosition(m_camera.GetPosition());
                m_meshes[0].Render(m_camera.GetProjection() * m_camera.GetView());
            }
        }

        if (!OpenGL::ToolWindow::WaterSceneEnabled && !OpenGL::ToolWindow::SpaceSceneEnabled)
        {
            for (unsigned int count = 0; count < Mesh::Lights.size(); count++)
            {
                Mesh::Lights[count].Render(m_camera.GetProjection() * m_camera.GetView());
            }
        }

        if (OpenGL::ToolWindow::WireframeRenderEnabled)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

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

        f.RenderText(fpsS, 100, 100, 0.5f, {1.0f, 1.0f, 0.0f});

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        char mousePosBuffer[128];
        sprintf_s(mousePosBuffer, "Mouse Pos  : %.6f   %.6f", mouseX, mouseY);
        string mousePosStr = mousePosBuffer;
        f.RenderText(mousePosStr, 100, 130, 0.5f, {1.0f, 1.0f, 0.0f});

        int leftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        int middleButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
        string leftButtonStr = (leftButtonState == GLFW_PRESS) ? "Left Button: Down" : "Left Button: Up";
        string middleButtonStr = (middleButtonState == GLFW_PRESS) ? "Middle Button: Down" : "Middle Button: Up";
        f.RenderText(leftButtonStr, 100, 160, 0.5f, {1.0f, 1.0f, 0.0f});
        f.RenderText(middleButtonStr, 100, 190, 0.5f, {1.0f, 1.0f, 0.0f});

        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            glm::vec3 fishPos = m_waterPlane.GetPosition();
            glm::vec3 fishRot = m_waterPlane.GetRotation();
            glm::vec3 fishScale = m_waterPlane.GetScale();

            char posBuffer[256];
            char rotBuffer[256];
            char scaleBuffer[256];

            sprintf_s(posBuffer, "Fish Position: X:%.2f Y:%.2f Z:%.2f", fishPos.x, fishPos.y, fishPos.z);
            sprintf_s(rotBuffer, "Fish Rotation: X:%.2f Y:%.2f Z:%.2f", fishRot.x, fishRot.y, fishRot.z);
            sprintf_s(scaleBuffer, "Fish Scale   : X:%.2f Y:%.2f Z:%.2f", fishScale.x, fishScale.y, fishScale.z);

            f.RenderText(string(posBuffer), 100, 220, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(rotBuffer), 100, 250, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(scaleBuffer), 100, 280, 0.5f, {1.0f, 1.0f, 0.0f});
        }
        else if (m_meshes.size() > 0)
        {
            glm::vec3 fighterPos = m_meshes[0].GetPosition();
            glm::vec3 fighterRot = m_meshes[0].GetRotation();
            glm::vec3 fighterScale = m_meshes[0].GetScale();

            char posBuffer[256];
            char rotBuffer[256];
            char scaleBuffer[256];

            sprintf_s(posBuffer, "Fighter Position: X:%.2f Y:%.2f Z:%.2f", fighterPos.x, fighterPos.y, fighterPos.z);
            sprintf_s(rotBuffer, "Fighter Rotation: X:%.2f Y:%.2f Z:%.2f", fighterRot.x, fighterRot.y, fighterRot.z);
            sprintf_s(scaleBuffer, "Fighter Scale   : X:%.2f Y:%.2f Z:%.2f", fighterScale.x, fighterScale.y,
                      fighterScale.z);

            f.RenderText(string(posBuffer), 100, 220, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(rotBuffer), 100, 250, 0.5f, {1.0f, 1.0f, 0.0f});
            f.RenderText(string(scaleBuffer), 100, 280, 0.5f, {1.0f, 1.0f, 0.0f});
        }

        if (OpenGL::ToolWindow::WaterSceneEnabled)
        {
            glUseProgram(m_shaderPost.GetProgramID());
            m_shaderPost.SetFloat("Time", static_cast<float>(m_gameTime));
            m_shaderPost.SetFloat("Frequency", OpenGL::ToolWindow::WaterScaleFrequency);
            m_shaderPost.SetFloat("Amplitude", OpenGL::ToolWindow::WaterScaleAmplitude);
            glUseProgram(0);
            m_postProcessor.End();
        }

        glfwSwapBuffers(window);
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


    f.Cleanup();
    m_postProcessor.Cleanup();
    m_skybox.Cleanup();
    m_waterPlane.Cleanup();
    m_spaceship.Cleanup();
    m_asteroids.Cleanup();
    m_shaderDiffuse.Cleanup();
    m_shaderColor.Cleanup();
    m_shaderFont.Cleanup();
    m_shaderPost.Cleanup();
    m_shaderWater.Cleanup();
    m_shaderSkybox.Cleanup();
}
