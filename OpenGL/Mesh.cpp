#include "Mesh.h"
#include "Shader.h"

Mesh::Mesh()
{
	m_shader = nullptr;
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_texture = { };
    m_texture2 = { };
    m_position = { 0, 0, 0 };
    m_rotation = { 0, 0, 0 };
    
}

Mesh::~Mesh()
{
}

void Mesh::Create(Shader* _shader)
{
    m_shader = _shader;
    m_texture = Texture();
    m_texture.LoadTexture("Assets/Textures/Wood.jpg");
    m_texture2 = Texture();
    m_texture2.LoadTexture("Assets/Textures/Emoji.jpg");

    m_vertexData = {
        /*     Position     */   /*  RGBA Color  */   /* Texture Coords */
        50.0f,  50.0f, 0.0f,     1.0f, 0.0f, 0.0f,     1.0f, 1.0f,   // top-right
        50.0f, -50.0f, 0.0f,     0.0f, 1.0f, 0.0f,     1.0f, 0.0f,   // bottom-right
       -50.0f, -50.0f, 0.0f,     0.0f, 0.0f, 1.0f,     0.0f, 0.0f,   // bottom-left
       -50.0f,  50.0f, 0.0f,     1.0f, 1.0f, 1.0f,     0.0f, 1.0f    // top-left
   };


    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);

    m_indexData = {
        2, 0, 3, 2, 1, 0
    };
    
    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size() * sizeof(GLubyte), m_indexData.data(), GL_STATIC_DRAW);
}



void Mesh::Cleanup()
{
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteBuffers(1, &m_vertexBuffer);
    m_texture.Cleanup();
    m_texture2.Cleanup();
}

void Mesh::Render(glm::mat4 _wvp)
{
    glUseProgram(m_shader->GetProgramID()); // Use our shader

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(m_shader->GetAttrVertices());
    glVertexAttribPointer(m_shader->GetAttrVertices(), // The attribute we want to configure
                          3,                           // size (3 vertices per primitive)
                          GL_FLOAT,                    // type
                          GL_FALSE,                    // normalized?
                          8 * sizeof(float),           // stride (8 floats per vertex definition)
                          (void*)0);                   // array buffer offset

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(m_shader->GetAttrColors());
    glVertexAttribPointer(m_shader->GetAttrColors(), // The attribute we want to configure
                          3,                         // size (3 vertices per primitive)
                          GL_FLOAT,                  // type
                          GL_FALSE,                  // normalized?
                          8 * sizeof(float),         // stride (8 floats per vertex definition)
                          (void*)(3 * sizeof(float))); // array buffer offset

    // 3rd attribute buffer : texCoords
    glEnableVertexAttribArray(m_shader->GetAttrTexCoords());
    glVertexAttribPointer(m_shader->GetAttrTexCoords(), // The attribute we want to configure
                          2,                            // size (3 vertices per primitive)
                          GL_FLOAT,                     // type
                          GL_FALSE,                     // normalized?
                          8 * sizeof(float),            // stride (7 floats per vertex definition)
                          (void*)(6 * sizeof(float)));  // array buffer offset

    // 4th attribute : WVP
    m_rotation.y += 0.0001f;
    glm::mat4 transform = glm::rotate(_wvp, m_rotation.y, glm::vec3(0, 1, 0));
    glUniformMatrix4fv(m_shader->GetAttrWVP(), 1, GL_FALSE, &transform[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);      // Bind the vertex buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer); // Bind the index buffer
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture.GetTexture());
    glUniform1i(m_shader->GetSampler1(), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2.GetTexture());
    glUniform1i(m_shader->GetSampler2(), 1);


    // glBindTexture(GL_TEXTURE_2D, m_texture.GetTexture());
    // glDrawElements(GL_TRIANGLES, m_indexData.size(), GL_UNSIGNED_BYTE, (void*)0);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_indexData.size(), GL_UNSIGNED_BYTE, (void*)0);
    glDisableVertexAttribArray(m_shader->GetAttrColors());
    glDisableVertexAttribArray(m_shader->GetAttrVertices());
    glDisableVertexAttribArray(m_shader->GetAttrTexCoords());
}
