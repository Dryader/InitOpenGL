#ifndef MESH_H
#define MESH_H

#include "StandardIncludes.h"
#include "Texture.h"

class Shader;

class Mesh
{
public:
    // Constructors / Destructors
    Mesh();
    virtual ~Mesh();

    //Accessors
    void SetPosition(glm::vec3 _position) { m_position = _position; }
    glm::vec3 GetPosition() { return m_position; }
    void SetScale(glm::vec3 _scale) { m_scale = _scale; }
    void SetLightPosition(glm::vec3 _lightPosition) { m_lightPosition = _lightPosition; }
    void SetLightColor(glm::vec3 _lightColor) { m_lightColor = _lightColor; }
    void SetCameraPosition(glm::vec3 _cameraPosition) { m_cameraPosition = _cameraPosition; }

    // Methods
    void Create(Shader* _shader);
    void Cleanup();
    void CalculateTransform();
    void Render(glm::mat4 _pv);

private:
    void SetShaderVariables(glm::mat4 _pv);
    void BindAttributes();


    Shader* m_shader;
    Texture m_texture;
    Texture m_texture2;
    GLuint m_vertexBuffer;
    GLuint m_indexBuffer;
    std::vector<GLfloat> m_vertexData;
    std::vector<GLubyte> m_indexData;

    glm::vec3 m_position;
    glm::vec3 m_rotation;
    glm::vec3 m_scale;
    glm::mat4 m_world;

    glm::vec3 m_lightPosition;
    glm::vec3 m_lightColor;
    glm::vec3 m_cameraPosition;
};


#endif // MESH_H
