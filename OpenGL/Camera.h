#ifndef CAMERA_H
#define CAMERA_H

#include "StandardIncludes.h"

class Camera
{
public:
    // Constructors / Destructors
    Camera();
    Camera(Resolution _screenResolution);
    virtual ~Camera();

    // Accessors
    glm::mat4 GetProjection() { return m_projection; }
    glm::mat4 GetView() { return m_view; }
    glm::vec3 GetPosition() { return m_position; }

    void Rotate();
    void ResetRotation();  // Reset camera rotation angle

private:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_position;
    glm::vec3 m_lookAt;
    glm::vec3 m_rotation;
    float m_angle;
};

#endif // CAMERA_H
