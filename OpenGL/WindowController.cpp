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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    M_ASSERT((m_window = glfwCreateWindow(1024, 768, "A sample scene", NULL, NULL)) != nullptr, "Failed to open GLFW window.");
    glfwMakeContextCurrent(m_window);
}
