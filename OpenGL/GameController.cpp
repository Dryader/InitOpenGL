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

	//Create a default perspective camera
	m_camera = Camera(WindowController::GetInstance().GetResolution());
}

void GameController::RunGame()
{

	//Create and compile our GLSL program from the shaders
	m_shader = Shader();
	m_shader.LoadShaders("Shaders/SimpleVertexShader.vertexshader.txt", "Shaders/SimpleFragmentShader.fragmentshader.txt");
    m_mesh = Mesh();
    m_mesh.Create(&m_shader);
	
    GLFWwindow* window = WindowController::GetInstance().GetWindow();
    int currentView = 0;
    int tabKeyState = GLFW_RELEASE;

    do
    {
        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && tabKeyState == GLFW_RELEASE) {
            currentView = (currentView + 1) % 4;
        }
        tabKeyState = glfwGetKey(window, GLFW_KEY_TAB);

        glm::mat4 view;
        glm::mat4 projection;

        switch (currentView)
        {
        case 0: // Perspective view 1
            projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
            view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            break;
        case 1: // Perspective view 2
            projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
            view = glm::lookAt(glm::vec3(-4, -3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            break;
        case 2: // Orthographic view 1
            projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
            view = glm::lookAt(glm::vec3(4, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            break;
        case 3: // Orthographic view 2
            projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
            view = glm::lookAt(glm::vec3(-10, -3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
    	m_mesh.Render(projection * view);
        glfwSwapBuffers(WindowController::GetInstance().GetWindow()); // Swap the front and back buffers
        glfwPollEvents();
    }
    while (glfwGetKey(WindowController::GetInstance().GetWindow(), GLFW_KEY_ESCAPE) != GLFW_PRESS );
    	// Check if the ESC key was pressed

    m_mesh.Cleanup();
	m_shader.Cleanup();
}
