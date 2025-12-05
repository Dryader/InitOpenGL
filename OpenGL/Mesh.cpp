#include "Mesh.h"
#include "Shader.h"
#include "OBJ_Loader.h"
#include "ASEMesh.h"
#include "ToolWindow.h"
#include <msclr/marshal_cppstd.h>

using namespace ASEMeshes;
using namespace OpenGL;

vector<Mesh> Mesh::Lights;

Mesh::Mesh()
{
    m_shader = nullptr;
    m_texture = {};
    m_texture2 = {};
    m_vertexBuffer = 0;
    m_indexBuffer = 0;
    m_instanceBuffer = 0;
    m_enableNormalMap = false;
    m_position = {0, 0, 0};
    m_rotation = {0, 0, 0};
    m_scale = {1, 1, 1};
    m_world = glm::mat4();
    m_instanceCount = 1;
    m_enableInstancing = false;
    m_elementSize = 0;
    m_color = {1, 1, 1};
    m_instancePositions.clear();
    m_instanceRotations.clear();
    m_instanceScales.clear();
    m_instanceRotationVelocities.clear();
}

Mesh::~Mesh()
{
}

static bool EndsWith(const string& _str, const string& _suffix)
{
    if (_suffix.size() > _str.size()) return false;
    return _str.compare(_str.size() - _suffix.size(), _suffix.size(), _suffix) == 0;
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

void Mesh::CalculateTangents(vector<objl::Vertex> _vertices, objl::Vector3& _tangent, objl::Vector3& _bitangent)
{
    objl::Vector3 edge1 = _vertices[1].Position - _vertices[0].Position;
    objl::Vector3 edge2 = _vertices[2].Position - _vertices[0].Position;
    objl::Vector2 deltaUV1 = _vertices[1].TextureCoordinate - _vertices[0].TextureCoordinate;
    objl::Vector2 deltaUV2 = _vertices[2].TextureCoordinate - _vertices[0].TextureCoordinate;

    float det = deltaUV1.X * deltaUV2.Y - deltaUV2.X * deltaUV1.Y;
    if (fabs(det) < 0.00001f)
    {
        objl::Vector3 normal = _vertices[0].Normal;
        
        objl::Vector3 up = (fabs(normal.Y) < 0.9f) ? objl::Vector3(0, 1, 0) : objl::Vector3(1, 0, 0);
        
        _tangent.X = normal.Y * up.Z - normal.Z * up.Y;
        _tangent.Y = normal.Z * up.X - normal.X * up.Z;
        _tangent.Z = normal.X * up.Y - normal.Y * up.X;
        
        float tanLen = sqrt(_tangent.X * _tangent.X + _tangent.Y * _tangent.Y + _tangent.Z * _tangent.Z);
        if (tanLen > 0.00001f)
        {
            _tangent.X /= tanLen;
            _tangent.Y /= tanLen;
            _tangent.Z /= tanLen;
        }
        
        _bitangent.X = normal.Y * _tangent.Z - normal.Z * _tangent.Y;
        _bitangent.Y = normal.Z * _tangent.X - normal.X * _tangent.Z;
        _bitangent.Z = normal.X * _tangent.Y - normal.Y * _tangent.X;
        
        return;
    }

    float f = 1.0f / det;

    _tangent.X = f * (deltaUV2.Y * edge1.X - deltaUV1.Y * edge2.X);
    _tangent.Y = f * (deltaUV2.Y * edge1.Y - deltaUV1.Y * edge2.Y);
    _tangent.Z = f * (deltaUV2.Y * edge1.Z - deltaUV1.Y * edge2.Z);

    _bitangent.X = f * (-deltaUV2.X * edge1.X + deltaUV1.X * edge2.X);
    _bitangent.Y = f * (-deltaUV2.X * edge1.Y + deltaUV1.X * edge2.Y);
    _bitangent.Z = f * (-deltaUV2.X * edge1.Z + deltaUV1.X * edge2.Z);

    float tanLen = sqrt(_tangent.X * _tangent.X + _tangent.Y * _tangent.Y + _tangent.Z * _tangent.Z);
    if (tanLen > 0.00001f)
    {
        _tangent.X /= tanLen;
        _tangent.Y /= tanLen;
        _tangent.Z /= tanLen;
    }
    else
    {
        _tangent.X = 1.0f;
        _tangent.Y = 0.0f;
        _tangent.Z = 0.0f;
    }
    
    float bitLen = sqrt(_bitangent.X * _bitangent.X + _bitangent.Y * _bitangent.Y + _bitangent.Z * _bitangent.Z);
    if (bitLen > 0.00001f)
    {
        _bitangent.X /= bitLen;
        _bitangent.Y /= bitLen;
        _bitangent.Z /= bitLen;
    }
    else
    {
        _bitangent.X = 0.0f;
        _bitangent.Y = 1.0f;
        _bitangent.Z = 0.0f;
    }
}


void Mesh::Cleanup()
{
    if (m_vertexBuffer != 0)
    {
        glDeleteBuffers(1, &m_vertexBuffer);
        m_vertexBuffer = 0;
    }
    
    if (m_indexBuffer != 0)
    {
        glDeleteBuffers(1, &m_indexBuffer);
        m_indexBuffer = 0;
    }
    
    if (m_instanceBuffer != 0)
    {
        glDeleteBuffers(1, &m_instanceBuffer);
        m_instanceBuffer = 0;
    }
    
    m_textureDiffuse.Cleanup();
    m_textureSpecular.Cleanup();
    m_textureNormal.Cleanup();
    m_texture.Cleanup();
    m_texture2.Cleanup();
}

#pragma managed
void Mesh::LoadASEFile(string _file)
{
    ASEMesh^ meshData = gcnew ASEMesh(_file.c_str());
    meshData->ParseASEFile();
    MeshInfo^ m = meshData->GeoObjects[0]->MeshI;
    Material^ mat = meshData->Materials[meshData->GeoObjects[0]->MaterialID];

    vector<objl::Vector3> tangents;
    vector<objl::Vector3> bitangents;
    vector<objl::Vertex> triangle;
    objl::Vector3 tangent;
    objl::Vector3 bitangent;
    int vCount = 0;
    for (int count = 0; count < m->NumFaces; count++)
    {
        Vec3^ tF = m->TexFaces[count];
        Vec3^ f = m->Faces[count];
        triangle.clear();

        objl::Vertex vert = objl::Vertex();
        vert.Position = objl::Vector3(m->Vertices[(int)f->X]->X, m->Vertices[(int)f->X]->Y,
                                      m->Vertices[(int)f->X]->Z);
        vert.Normal = objl::Vector3(m->VertexNormals[vCount]->X, m->VertexNormals[vCount]->Y,
                                    m->VertexNormals[vCount]->Z);
        vert.TextureCoordinate = objl::Vector2(m->TexVertices[(int)tF->X]->X, m->TexVertices[(int)tF->X]->Y);
        triangle.push_back(vert);
        vCount++;

        vert = objl::Vertex();
        vert.Position = objl::Vector3(m->Vertices[(int)f->Y]->X, m->Vertices[(int)f->Y]->Y,
                                      m->Vertices[(int)f->Y]->Z);
        vert.Normal = objl::Vector3(m->VertexNormals[vCount]->X, m->VertexNormals[vCount]->Y,
                                    m->VertexNormals[vCount]->Z);
        vert.TextureCoordinate = objl::Vector2(m->TexVertices[(int)tF->Y]->X, m->TexVertices[(int)tF->Y]->Y);
        triangle.push_back(vert);
        vCount++;

        vert = objl::Vertex();
        vert.Position = objl::Vector3(m->Vertices[(int)f->Z]->X, m->Vertices[(int)f->Z]->Y,
                                      m->Vertices[(int)f->Z]->Z);
        vert.Normal = objl::Vector3(m->VertexNormals[vCount]->X, m->VertexNormals[vCount]->Y,
                                    m->VertexNormals[vCount]->Z);
        vert.TextureCoordinate = objl::Vector2(m->TexVertices[(int)tF->Z]->X, m->TexVertices[(int)tF->Z]->Y);
        triangle.push_back(vert);
        vCount++;

        CalculateTangents(triangle, tangent, bitangent);
        tangents.push_back(tangent);
        bitangents.push_back(bitangent);

        for (int c = 0; c < 3; c++)
        {
            m_vertexData.push_back(triangle[c].Position.X);
            m_vertexData.push_back(triangle[c].Position.Y);
            m_vertexData.push_back(triangle[c].Position.Z);
            m_vertexData.push_back(triangle[c].Normal.X);
            m_vertexData.push_back(triangle[c].Normal.Y);
            m_vertexData.push_back(triangle[c].Normal.Z);
            m_vertexData.push_back(triangle[c].TextureCoordinate.X);
            m_vertexData.push_back(triangle[c].TextureCoordinate.Y);

            int index = (vCount / 3) - 1;
            m_vertexData.push_back(tangents[index].X);
            m_vertexData.push_back(tangents[index].Y);
            m_vertexData.push_back(tangents[index].Z);
            m_vertexData.push_back(bitangents[index].X);
            m_vertexData.push_back(bitangents[index].Y);
            m_vertexData.push_back(bitangents[index].Z);
        }
    }

    m_textureDiffuse = Texture();
    if (mat->Maps[0]->Name == "DIFFUSE")
    {
        string fn = msclr::interop::marshal_as<std::string>(mat->Maps[0]->TextureFileName);
        m_textureDiffuse.LoadTexture("Assets/Textures/" + RemoveFolder(fn));
    }
    m_textureSpecular = Texture();
    if (mat->Maps[1]->Name == "SPECULAR")
    {
        string fn = msclr::interop::marshal_as<std::string>(mat->Maps[1]->TextureFileName);
        m_textureSpecular.LoadTexture("Assets/Textures/" + RemoveFolder(fn));
    }
    m_textureNormal = Texture();
    if (mat->Maps[1]->Name == "BUMP")
    {
        string fn = msclr::interop::marshal_as<std::string>(mat->Maps[1]->TextureFileName);
        m_textureNormal.LoadTexture("Assets/Textures/" + RemoveFolder(fn));
        m_enableNormalMap = true;
    }
    else if (mat->Maps[2]->Name == "BUMP")
    {
        string fn = msclr::interop::marshal_as<std::string>(mat->Maps[2]->TextureFileName);
        m_textureNormal.LoadTexture("Assets/Textures/" + RemoveFolder(fn));
        m_enableNormalMap = true;
    }
}

void Mesh::GetSpecularSettings(float& _strength, float& _colorR, float& _colorG, float& _colorB)
{
    using namespace OpenGL;
    _strength = ToolWindow::SpecularStrength;
    _colorR = ToolWindow::SpecularColorR;
    _colorG = ToolWindow::SpecularColorG;
    _colorB = ToolWindow::SpecularColorB;
}
#pragma unmanaged

bool Mesh::Create(Shader* _shader, string _file, int _instanceCount)
{
    m_shader = _shader;
    m_instanceCount = _instanceCount;
    if (m_instanceCount > 1)
    {
        m_enableInstancing = true;
    }

#pragma region LoadMesh
    objl::Loader Loader;

    if (EndsWith(_file, ".ase"))
    {
        LoadASEFile(_file);
    }
    else
    {
        M_ASSERT(Loader.LoadFile(_file) == true, "Failed to load mesh.");

        for (unsigned int meshIdx = 0; meshIdx < Loader.LoadedMeshes.size(); meshIdx++)
        {
            objl::Mesh& curMesh = Loader.LoadedMeshes[meshIdx];

            for (unsigned int i = 0; i < curMesh.Indices.size(); i += 3)
            {
                vector<objl::Vertex> triangle;
                objl::Vector3 tangent;
                objl::Vector3 bitangent;

                for (int j = 0; j < 3; j++)
                {
                    unsigned int idx = curMesh.Indices[i + j];
                    triangle.push_back(curMesh.Vertices[idx]);
                }

                CalculateTangents(triangle, tangent, bitangent);

                for (int j = 0; j < 3; j++)
                {
                    m_vertexData.push_back(triangle[j].Position.X);
                    m_vertexData.push_back(triangle[j].Position.Y);
                    m_vertexData.push_back(triangle[j].Position.Z);
                    m_vertexData.push_back(triangle[j].Normal.X);
                    m_vertexData.push_back(triangle[j].Normal.Y);
                    m_vertexData.push_back(triangle[j].Normal.Z);
                    m_vertexData.push_back(triangle[j].TextureCoordinate.X);
                    m_vertexData.push_back(triangle[j].TextureCoordinate.Y);
                    m_vertexData.push_back(tangent.X);
                    m_vertexData.push_back(tangent.Y);
                    m_vertexData.push_back(tangent.Z);
                    m_vertexData.push_back(bitangent.X);
                    m_vertexData.push_back(bitangent.Y);
                    m_vertexData.push_back(bitangent.Z);
                }
            }
        }
    }
#pragma endregion

    if (!EndsWith(_file, ".ase"))
    {
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
    }

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(float), m_vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (m_enableInstancing)
    {
        glGenBuffers(1, &m_instanceBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);

        srand(static_cast<unsigned int>(glfwGetTime()));
        for (unsigned int i = 0; i < m_instanceCount; i++)
        {
            glm::vec3 position = glm::vec3(-8 + (rand() % 16), -8 + (rand() % 16), -8 + (rand() % 16));
            m_instancePositions.push_back(position);

            glm::vec3 rotation = glm::vec3(static_cast<float>(rand() % 360),
                                           static_cast<float>(rand() % 360),
                                           static_cast<float>(rand() % 360));
            m_instanceRotations.push_back(rotation);

            float baseScale = 0.01f;
            float randomScale = 0.5f + (rand() % 100) / 100.0f;
            m_instanceScales.push_back(glm::vec3(baseScale * randomScale, baseScale * randomScale,
                                                 baseScale * randomScale));

            glm::vec3 rotationVelocity = glm::vec3((rand() % 50) / 100.0f - 0.25f,
                                                   (rand() % 50) / 100.0f - 0.25f,
                                                   (rand() % 50) / 100.0f - 0.25f);
            m_instanceRotationVelocities.push_back(rotationVelocity);

            auto model = glm::mat4(1.0f);
            model = glm::translate(model, position);
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
            model = glm::scale(model, m_instanceScales[i]);

            for (int x = 0; x < 4; x++)
            {
                for (int y = 0; y < 4; y++)
                {
                    m_instanceData.push_back(model[x][y]);
                }
            }
        }
        glBufferData(GL_ARRAY_BUFFER, m_instanceCount * sizeof(glm::mat4), m_instanceData.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    return true;
}


void Mesh::CalculateTransform()
{
    m_world = glm::translate(glm::mat4(1.0f), m_position);
    m_world = glm::rotate(m_world, glm::radians(m_rotation.x), glm::vec3(1, 0, 0));
    m_world = glm::rotate(m_world, glm::radians(m_rotation.y), glm::vec3(0, 1, 0));
    m_world = glm::rotate(m_world, glm::radians(m_rotation.z), glm::vec3(0, 0, 1));
    m_world = glm::scale(m_world, m_scale);
}

void Mesh::UpdateInstanceRotations(float _deltaTime)
{
    if (!m_enableInstancing)
        return;

    m_instanceData.clear();

    for (unsigned int i = 0; i < m_instanceCount; i++)
    {
        m_instanceRotations[i] += m_instanceRotationVelocities[i];

        auto model = glm::mat4(1.0f);
        model = glm::translate(model, m_instancePositions[i]);
        model = glm::rotate(model, glm::radians(m_instanceRotations[i].x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(m_instanceRotations[i].y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(m_instanceRotations[i].z), glm::vec3(0, 0, 1));
        model = glm::scale(model, m_instanceScales[i]);

        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                m_instanceData.push_back(model[x][y]);
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceCount * sizeof(glm::mat4), m_instanceData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::SetShaderVariables(glm::mat4 _pv)
{
    m_shader->SetMat4("World", m_world);
    m_shader->SetMat4("WVP", _pv * m_world);
    m_shader->SetVec3("CameraPosition", m_cameraPosition);
    m_shader->SetInt("EnableNormalMap", m_enableNormalMap);
    m_shader->SetInt("EnableInstancing", m_enableInstancing);

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

    float specularStrength;
    float specularColorR;
    float specularColorG;
    float specularColorB;
    GetSpecularSettings(specularStrength, specularColorR, specularColorG, specularColorB);

    m_shader->SetFloat("material.specularStrength", specularStrength);
    m_shader->SetVec3("light[0].specularColor", {specularColorR, specularColorG, specularColorB});
    m_shader->SetTextureSampler("material.diffuseTexture", GL_TEXTURE0, 0, m_textureDiffuse.GetTexture());
    m_shader->SetTextureSampler("material.specularTexture", GL_TEXTURE1, 1, m_textureSpecular.GetTexture());
    m_shader->SetTextureSampler("material.normalTexture", GL_TEXTURE2, 2, m_textureNormal.GetTexture());
}


void Mesh::Render(glm::mat4 _pv)
{
    glUseProgram(m_shader->GetProgramID());

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

    glDisableVertexAttribArray(m_shader->GetAttrVertices());
    glDisableVertexAttribArray(m_shader->GetAttrNormals());
    glDisableVertexAttribArray(m_shader->GetAttrTexCoords());

    if (m_shader->GetAttrTangents() != -1)
    {
        glDisableVertexAttribArray(m_shader->GetAttrTangents());
    }
    
    if (m_shader->GetAttrBitangents() != -1)
    {
        glDisableVertexAttribArray(m_shader->GetAttrBitangents());
    }

    if (m_enableInstancing)
    {
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix());
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 1);
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 2);
        glDisableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 3);

        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix(), 0);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 1, 0);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 2, 0);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 3, 0);
    }
}


void Mesh::BindAttributes()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    int stride = 14;
    m_elementSize = 14;

    glEnableVertexAttribArray(m_shader->GetAttrVertices());
    glVertexAttribPointer(m_shader->GetAttrVertices(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          static_cast<void*>(nullptr));

    glEnableVertexAttribArray(m_shader->GetAttrNormals());
    glVertexAttribPointer(m_shader->GetAttrNormals(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(m_shader->GetAttrTexCoords());
    glVertexAttribPointer(m_shader->GetAttrTexCoords(), 2, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                          (void*)(6 * sizeof(float)));

    if (m_shader->GetAttrTangents() != -1)
    {
        glEnableVertexAttribArray(m_shader->GetAttrTangents());
        glVertexAttribPointer(m_shader->GetAttrTangents(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              (void*)(8 * sizeof(float)));
    }

    if (m_shader->GetAttrBitangents() != -1)
    {
        glEnableVertexAttribArray(m_shader->GetAttrBitangents());
        glVertexAttribPointer(m_shader->GetAttrBitangents(), 3, GL_FLOAT, GL_FALSE, stride * sizeof(float),
                              (void*)(11 * sizeof(float)));
    }
    if (m_enableInstancing)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceBuffer);
        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix());
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix(), 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              static_cast<void*>(nullptr));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 1);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(sizeof(glm::vec4)));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 2);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(m_shader->GetAttrInstanceMatrix() + 3);
        glVertexAttribPointer(m_shader->GetAttrInstanceMatrix() + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix(), 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 1, 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 2, 1);
        glVertexAttribDivisor(m_shader->GetAttrInstanceMatrix() + 3, 1);
    }
}
