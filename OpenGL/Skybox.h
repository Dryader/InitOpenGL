#ifndef SKYBOX_H
#define SKYBOX_H

#include "Mesh.h"

class Skybox
{
public:
    Skybox();
    virtual ~Skybox();

    bool Create(Shader* _shader, string _file, vector<string> _faces);
    void Cleanup();
    void Render(glm::mat4 _pv);

private:
    void SetShaderVariables(glm::mat4 _pv);
    void BindAttributes();

    Shader* m_shader;
    Texture m_texture;
    GLuint m_vertexBuffer;
    std::vector<GLfloat> m_vertexData;
};

#endif
