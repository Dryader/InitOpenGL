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
    m_instanceCount = 1;
    m_enableInstancing = false;
    m_elementSize = 0;
}

Mesh::~Mesh()
{
}

string Mesh::Concat(string _s1, int _index, string _s2)
{
    string index = to_string(_index);
    return (_s1 + index + _s2);
}

string Mesh::RemoveFolder(string _map)
{
    const size_t last_slash_idx = _map.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        _map.erase(0, last_slash_idx + 1);
    }
    return _map;
}

void Mesh::CalculateTangents(vector<objl::Vertex> _vertices, objl::Vector3& _tanget, objl::Vector3& _bitanget)
{
    //calculate tanget/bitanget vectors of both triangles
    objl::Vector3 edge1 = _vertices[1].Position - _vertices[0].Position;
    objl::Vector3 edge2 = _vertices[2].Position - _vertices[0].Position;
    objl::Vector2 deltaUV1 = _vertices[1].TextureCoordinate - _vertices[0].TextureCoordinate;
    objl::Vector2 deltaUV2 = _vertices[2].TextureCoordinate - _vertices[0].TextureCoordinate;

    float f = 1.0f / (deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y);

    _tanget.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
    _tanget.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
    _tanget.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);

    _bitanget.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
    _bitanget.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
    _bitanget.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);
}


void Mesh::Cleanup()
{
    glDeleteBuffers(1, &m_indexBuffer);
    m_texture.Cleanup();
    m_texture2.Cleanup();
}

bool Mesh::Create(Shader* _shader, string _file, int _instanceCount)
{
    m_shader = _shader;
    m_instanceCount = _instanceCount;
    if (m_instanceCount > 1)
    {
        m_enableInstancing = true;
    }

    objl::Loader Loader; // Initialize Loader
    bool loaded = Loader.LoadFile(_file);
    if (!loaded)
    {
        return false;
    }

    for (unsigned int i = 0; i < Loader.LoadedMeshes.size(); i++)
    {
        objl::Mesh curMesh = Loader.LoadedMeshes[i];
        vector<objl::Vector3> tangents;
        vector<objl::Vector3> bitangets;
        vector<objl::Vertex> triangle;
        objl::Vector3 tangent;
        objl::Vector3 bitanget;
        for (unsigned int j = 0; j < curMesh.Vertices.size(); j += 3)
        {
            triangle.clear();
            triangle.push_back(curMesh.Vertices[j]);
            triangle.push_back(curMesh.Vertices[j + 1]);
            triangle.push_back(curMesh.Vertices[j + 2]);
            CalculateTangents(triangle, tangent, bitanget);
            tangents.push_back(tangent);
            bitangets.push_back(bitanget);
        }

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

            if (Loader.LoadedMaterials.size() > 0 && Loader.LoadedMaterials[0].map_bump != "")
            {
                int index = j / 3;
                if (index < tangents.size())
                {
                    m_vertexData.push_back(tangents[index].X);
                    m_vertexData.push_back(tangents[index].Y);
                    m_vertexData.push_back(tangents[index].Z);
                    m_vertexData.push_back(bitangets[index].X);
                    m_vertexData.push_back(bitangets[index].Y);
                    m_vertexData.push_back(bitangets[index].Z);
                }
            }
        }
    }

    // Remove directory if present and handle empty map_Kd safely
    // string diffuseNap = Loader.LoadedMaterials.size() > 0 ? Loader.LoadedMaterials[0].map_Kd : "";
    // if (!diffuseNap.empty())
    // {
    //     size_t last_backslash = diffuseNap.find_last_of("\\");
    //     size_t last_slash = diffuseNap.find_last_of("/");
    //     size_t last_sep = (last_backslash == std::string::npos)
    //                           ? last_slash
    //                           : ((last_slash == std::string::npos)
    //                                  ? last_backslash
    //                                  : std::max(last_backslash, last_slash));
    //     if (last_sep != std::string::npos)
    //     {
    //         diffuseNap.erase(0, last_sep + 1);
    //     }
    // }
    // else
    // {
    //     diffuseNap = "Wood.jpg"; // simple fallback texture name present in Assets/Textures
    // }

    m_textureDiffuse = Texture();
    if (Loader.LoadedMaterials.size() > 0 && Loader.LoadedMaterials[0].map_Kd != "")
    {
        m_textureDiffuse.LoadTexture("Assets/Textures/" + RemoveFolder(Loader.LoadedMaterials[0].map_Kd));
    }
    else
    {
        m_textureDiffuse.LoadTexture("Assets/Textures/Wood.jpg");
    }

    m_textureSpecular = Texture();
    if (Loader.LoadedMaterials.size() > 0 && Loader.LoadedMaterials[0].map_Ks != "")
    {
        m_textureSpecular.LoadTexture("Assets/Textures/" + RemoveFolder(Loader.LoadedMaterials[0].map_Ks));
    }
    else
    {
        m_textureSpecular.LoadTexture("Assets/Textures/Wood.jpg");
    }

    m_textureNormal = Texture();
    if (Loader.LoadedMaterials.size() > 0 && Loader.LoadedMaterials[0].map_bump != "")
    {
        m_textureNormal.LoadTexture("Assets/Textures/" + RemoveFolder(Loader.LoadedMaterials[0].map_bump));
        m_enableNormalMap = true;
    }

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_enableInstancing)
    {
        glGenBuffers(1, &m_instanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);

        srand(glfwGetTime());
        for (unsigned int i = 0; i < m_instanceCount; i++)
        {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-20 + rand() % 40, -10 + rand() % 20, 10 + rand() % 20));

            for (int x = 0; x < 4; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    m_instanceData.push_back(model[x][y]);
                }
            }
        }
        glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(glm::mat4), m_instanceData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}


void Mesh::CalculateTransform()
{
    m_world = glm::translate(glm::mat4(1.0f), m_position);
    m_world = glm::rotate(m_world, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    m_world = glm::scale(m_world, m_scale);
}

void Mesh::SetShaderVariables(glm::mat4 _pv)
{
    m_shader->SetMat4("World", m_world);
    m_shader->SetMat4("WVP", _pv * m_world);
    m_shader->SetVec3("CameraPosition", m_cameraPosition);
    m_shader->SetInt("EnableNormalMap", m_enableNormalMap);
    m_shader->SetInt("EnableInstancing", m_enableInstancing);

    // Configure light
    for (unsigned int i = 0; i < Lights.size(); i++)
    {
        m_shader->SetFloat(Concat("light[", i, "].constant").c_str(), 1.0f);
        m_shader->SetFloat(Concat("light[", i, "].linear").c_str(), 0.09f);
        m_shader->SetFloat(Concat("light[", i, "].quadratic").c_str(), 0.032f);

        m_shader->SetVec3(Concat("light[", i, "].ambientColor").c_str(), {0.1f, 0.1f, 0.1f});
        m_shader->SetVec3(Concat("light[", i, "].diffuseColor").c_str(), Lights[i].GetColor());
        m_shader->SetVec3(Concat("light[", i, "].specularColor").c_str(), {1.0f, 1.0f, 1.0f});

        m_shader->SetVec3(Concat("light[", i, "].position").c_str(), Lights[i].GetPosition());
        m_shader->SetVec3(Concat("light[", i, "].direction").c_str(),
                          glm::normalize(glm::vec3({0.0f + i * 0.1f, 0, 0.0f + i * 0.1f}) - Lights[i].GetPosition()));
        m_shader->SetFloat(Concat("light[", i, "].coneAngle").c_str(), glm::radians(5.0f));
        m_shader->SetFloat(Concat("light[", i, "].falloff").c_str(), 200);
    }

    // Configure material
    m_shader->SetFloat("material.specularStrength", 8);
    m_shader->SetTextureSampler("material.diffuseTexture", GL_TEXTURE0, 0, m_textureDiffuse.GetTexture());
    m_shader->SetTextureSampler("material.specularTexture", GL_TEXTURE1, 1, m_textureSpecular.GetTexture());
    m_shader->SetTextureSampler("material.normalTexture", GL_TEXTURE2, 2, m_textureNormal.GetTexture());
}


void Mesh::Render(glm::mat4 _pv)
{
    glUseProgram(m_shader->GetProgramID()); // Use our shader

    m_rotation.x += 0.1f;

    CalculateTransform();
    SetShaderVariables(_pv);
    BindAttributes();

    if (m_enableInstancing)
    {
        glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertexData.size() / m_elementSize, m_instanceCount);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, m_vertexData.size() / m_elementSize);
    }
    glDisableVertexAttribArray(m_shader->GetAttrNormals());
    glDisableVertexAttribArray(m_shader->GetAttrTexCoords());
    glDisableVertexAttribArray(m_shader->GetAttrTexCoords());

    if (m_enableNormalMap)
    {
        glDisableVertexAttribArray(m_shader->GetAttrTangents());
        glDisableVertexAttribArray(m_shader->GetAttrBitangents());
    }
    if (m_enableInstancing)
    {
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix());
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 1);
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 2);
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 3);
    }
}


void Mesh::BindAttributes()
{
    // Bind our vertex buffer first
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    int stride = 8;
    if (m_enableNormalMap)
    {
        stride += 6; // 3 for tangent, 3 for bitangent
    }
    glEnableVertexAttribArray(m_shader->GetAttrVertices());
    glVertexAttribPointer(m_shader->GetAttrVertices(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          static_cast<void*>(nullptr));

    glEnableVertexAttribArray(m_shader->GetAttrNormals());
    glVertexAttribPointer(m_shader->GetAttrNormals(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(m_shader->GetAttrTexCoords());
    glVertexAttribPointer(m_shader->GetAttrTexCoords(), 2, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void*)(6 * sizeof(float)));
    m_elementSize = 8;

    if (m_enableNormalMap)
    {
        glEnableVertexAttribArray(m_shader->GetAttrTangents());
        glVertexAttribPointer(m_shader->GetAttrTangents(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              (void*)(8 * sizeof(float)));

        glEnableVertexAttribArray(m_shader->GetAttrBitangents());
        glVertexAttribPointer(m_shader->GetAttrBitangents(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              (void*)(11 * sizeof(float)));
        m_elementSize += 6;
    }
    if (m_enableInstancing)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix());
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix(), 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::mat4),
                              static_cast<void*>(nullptr));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 1);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::mat4),
                              (void*)(sizeof(glm::vec4)));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 2);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::mat4),
                              (void*)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 3);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::mat4),
                              (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix(), 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 1, 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 2, 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 3, 1);
    }
}
