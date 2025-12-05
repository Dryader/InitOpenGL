#include "Camera.h"

Camera::Camera()
{
    m_projection = {};
    m_view = {};
    m_position = {};
    m_lookAt = glm::vec3();
    m_rotation = glm::vec3();
    m_angle = 0;
}

Camera::Camera(Resolution _screenResolution)
{
    m_position = {0, 0, 1.5f};
    m_lookAt = {0, 0, 0};
    m_rotation = {0, 0, 0};
    m_angle = 0;
    m_projection = glm::perspective(glm::radians(45.0f),
                                    static_cast<float>(_screenResolution.m_width) / static_cast<float>(_screenResolution
                                        .m_height), 0.1f,
                                    1000.0f);

    m_view = glm::lookAt(
        m_position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::Rotate()
{
    m_angle += 0.1f;
    m_lookAt.x = cos(glm::radians(m_angle)) * 100;
    m_lookAt.z = sin(glm::radians(m_angle)) * 100;
    m_view = glm::lookAt(
        m_position,
        m_lookAt,
        glm::vec3(0, 1, 0));
}

void Camera::ResetRotation()
{
    m_angle = 0.0f;
    m_lookAt = glm::vec3(0, 0, 0);
    m_view = glm::lookAt(
        m_position,
        m_lookAt,
        glm::vec3(0, 1, 0));
}

Camera::~Camera()
{
}
