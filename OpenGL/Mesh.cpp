#include "Mesh.h"

#include "Shader.h"

Mesh::Mesh()
{
	m_shader = nullptr;
    m_vertexBuffer = 0;
    m_world = glm::mat4(1.0f);
}

Mesh::~Mesh()
{
}

void Mesh::Create(Shader* _shader)
{
    m_shader = _shader;

    m_vertexData = { -1.0f, -1.0f, 0.0f,  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f };

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
}


void Mesh::Cleanup()
{
    glDeleteBuffers(1, &m_vertexBuffer);
}

void Mesh::Render(glm::mat4 _wvp)
{
    glUseProgram(m_shader->GetProgramID()); // Use our shader

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(m_shader->GetAttrVertices());
    glVertexAttribPointer(m_shader->GetAttrVertices(), // The attribute we want to configure
        m_vertexData.size() / 3,            // size
        GL_FLOAT,     // type
        GL_FALSE,     // normalized?
        0,            // stride
        (void*)0);    // array buffer offset

    _wvp *= m_world;
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glUniformMatrix4fv(m_shader->GetAttrWVP(), 1, GL_FALSE, &_wvp[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangle
    glDisableVertexAttribArray(m_shader->GetAttrVertices());
}
