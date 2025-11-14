#include "Mesh.h"
#include "Shader.h"
#include "OBJ_Loader.h"

vector<Mesh> Mesh::Lights;

Mesh::Mesh()
{
    m_shader = nullptr;
    m_texture = {};
    m_texture2 = {};
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_position = {0, 0, 0};
    m_rotation = {0, 0, 0};
    m_scale = {1, 1, 1};
    m_world = glm::mat4();
    m_cameraPosition = {0.0f, 0.0f, 0.0f};
    m_color = {1.0f, 1.0f, 1.0f};
}

Mesh::~Mesh()
{
}

string Mesh::Concat(string _s1, int _index, string _s2)
{
    string index = to_string(_index);
    return (_s1 + index + _s2);
}


void Mesh::Cleanup()
{
    glDeleteBuffers(1, &m_indexBuffer);
    m_texture.Cleanup();
    m_texture2.Cleanup();
}

bool Mesh::Create(Shader* _shader, string _file)
{
    m_shader = _shader;

    objl::Loader Loader; // Initialize Loader
    bool loaded = Loader.LoadFile(_file);
    if (!loaded)
    {
        return false;
    }

    for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
    {
        objl::Mesh curMesh = Loader.LoadedMeshes[i];
        for (unsigned int j = 0; j < curMesh.Vertices.size(); j++)
        {
            m_vertexData.push_back(curMesh.Vertices[j].Position.X);
            m_vertexData.push_back(curMesh.Vertices[j].Position.Y);
            m_vertexData.push_back(curMesh.Vertices[j].Position.Z);
            m_vertexData.push_back(curMesh.Vertices[j].Normal.X);
            m_vertexData.push_back(curMesh.Vertices[j].Normal.Y);
            m_vertexData.push_back(curMesh.Vertices[j].Normal.Z);
            m_vertexData.push_back(curMesh.Vertices[j].TextureCoordinate.X);
            m_vertexData.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
        }
    }

    // Remove directory if present and handle empty map_Kd safely
    string diffuseNap = Loader.LoadedMaterials.size() > 0 ? Loader.LoadedMaterials[0].map_Kd : "";
    if (!diffuseNap.empty())
    {
        size_t last_backslash = diffuseNap.find_last_of("\\");
        size_t last_slash = diffuseNap.find_last_of("/");
        size_t last_sep = (last_backslash == std::string::npos) ? last_slash : ((last_slash == std::string::npos) ? last_backslash : std::max(last_backslash, last_slash));
        if (last_sep != std::string::npos)
        {
            diffuseNap.erase(0, last_sep + 1);
        }
    }
    else
    {
        diffuseNap = "Wood.jpg"; // simple fallback texture name present in Assets/Textures
    }

    m_texture = Texture();
    m_texture.LoadTexture("Assets/Textures/" + diffuseNap);
    m_texture2 = Texture();
    m_texture2.LoadTexture("Assets/Textures/" + diffuseNap);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
    
    return true; // Success
}


void Mesh::CalculateTransform()
{
    m_world = glm::translate(glm::mat4(1.0f), m_position);
    m_world = glm::rotate(m_world, m_rotation.y, glm::vec3(0, 1, 0));
    m_world = glm::scale(m_world, m_scale);
}

void Mesh::SetShaderVariables(glm::mat4 _pv)
{
    m_shader->SetMat4("World", m_world);
    m_shader->SetMat4("WVP", _pv * m_world);
    m_shader->SetVec3("CameraPosition", m_cameraPosition);

    // Configure light
    for (unsigned int i = 0; i < Lights.size(); i++)
    {
        m_shader->SetFloat(Concat("light[", i, "].constant").c_str(), 1.0f);
        m_shader->SetFloat(Concat("light[", i, "].linear").c_str(), 0.09f);
        m_shader->SetFloat(Concat("light[", i, "].quadratic").c_str(), 0.032f);

        m_shader->SetVec3(Concat("light[", i, "].ambientColor").c_str(), {0.1f, 0.1f, 0.1f});
        m_shader->SetVec3(Concat("light[", i, "].diffuseColor").c_str(), Lights[i].GetColor());
        m_shader->SetVec3(Concat("light[", i, "].specularColor").c_str(), {3.0f, 3.0f, 3.0f});

        m_shader->SetVec3(Concat("light[", i, "].position").c_str(), Lights[i].GetPosition());
        m_shader->SetVec3(Concat("light[", i, "].direction").c_str(),
                          glm::normalize(glm::vec3({0.0f + i * 0.1f, 0, 0.0f + i * 0.1f}) - Lights[i].GetPosition()));
        m_shader->SetFloat(Concat("light[", i, "].coneAngle").c_str(), glm::radians(5.0f));
        m_shader->SetFloat(Concat("light[", i, "].falloff").c_str(), 200);
    }

    // Configure material
    m_shader->SetFloat("material.specularStrength", 8);
    m_shader->SetTextureSampler("material.diffuseTexture", GL_TEXTURE0, 0, m_texture.GetTexture());
    m_shader->SetTextureSampler("material.specularTexture", GL_TEXTURE1, 1, m_texture2.GetTexture());
}


void Mesh::Render(glm::mat4 _pv)
{
    glUseProgram(m_shader->GetProgramID()); // Use our shader

    m_rotation.y += 0.001f;

    CalculateTransform();
    SetShaderVariables(_pv);
    BindAttributes();

    // Each vertex has 8 floats (pos3 + normal3 + uv2)
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(m_vertexData.size() / 8));
    
    // Safely disable attributes
    if (m_shader->GetAttrNormals() != -1)
        glDisableVertexAttribArray(m_shader->GetAttrNormals());
    if (m_shader->GetAttrVertices() != -1)
        glDisableVertexAttribArray(m_shader->GetAttrVertices());
    if (m_shader->GetAttrTexCoords() != -1)
        glDisableVertexAttribArray(m_shader->GetAttrTexCoords());
}


void Mesh::BindAttributes()
{
    // Bind our vertex buffer first
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    // 1st attribute buffer : vertices
    GLint attrVertices = m_shader->GetAttrVertices();
    if (attrVertices != -1)
    {
        glEnableVertexAttribArray(attrVertices);
        glVertexAttribPointer(attrVertices, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    }

    // 2nd attribute buffer : normals
    GLint attrNormals = m_shader->GetAttrNormals();
    if (attrNormals != -1)
    {
        glEnableVertexAttribArray(attrNormals);
        glVertexAttribPointer(attrNormals, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    // 3rd attribute buffer : texCoords
    GLint attrTexCoords = m_shader->GetAttrTexCoords();
    if (attrTexCoords != -1)
    {
        glEnableVertexAttribArray(attrTexCoords);
        glVertexAttribPointer(attrTexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    }
}
