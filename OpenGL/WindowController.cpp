#include "WindowController.h"

WindowController::WindowController()
{
    m_window = nullptr;
}

WindowController::~WindowController()
{
    if (m_window != nullptr)
    {
        glfwTerminate();
        m_window = nullptr;
    }
}

void WindowController::NewWindow()
{
    M_ASSERT(glfwInit(), "Failed to initialize GLFW."); // Initialise GLFW

    // Open a window and create its OpenGL context
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Changed to 3.3 for better compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Request core profile
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on Mac, good practice on Windows
    M_ASSERT((m_window = glfwCreateWindow(1024, 768, "A sample scene", NULL, NULL)) != nullptr, "Failed to open GLFW window.");
    glfwMakeContextCurrent(m_window);
}

Resolution WindowController::GetResolution()
{
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    return Resolution(mode->width, mode->height);
}
