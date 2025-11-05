#include "Camera.h"

Camera::Camera()
{
    m_projection = { };
    m_view = { };
}

Camera::Camera(Resolution _screenResolution)
{
    m_projection = glm::perspective(glm::radians(45.0f),
        (float)_screenResolution.m_width / (float)_screenResolution.m_height,
        0.1f,
        1000.0f);

    // Camera at (200,200,200) looking at origin so the mesh at (100,100,0)
    // appears offset (top-right) instead of centered.
    m_view = glm::lookAt(
        glm::vec3(2, 2, 2), // eye
        glm::vec3(0.0f, 0.0f, 0.0f),       // target (origin)
        glm::vec3(0.0f, 1.0f, 0.0f));      // up
}

Camera::~Camera()
{
}
