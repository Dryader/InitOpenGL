#include "Shader.h"

Shader::Shader()
{
    m_programID = 0;
    m_attrVertices = 0;
    m_result = GL_FALSE;
    m_infoLogLength = 0;
    m_attrWVP = 0;
    m_attrColors = 0;
    m_attrNormals = 0;
    m_attrTexCoords = 0;
    m_attrTangents = 0;
    m_attrBitangents = 0;
    m_attrInstanceMatrix = 0;
}

void Shader::Cleanup()
{
    glDeleteProgram(m_programID);
}

void Shader::SetTextureSampler(const char* _name, GLuint _texUnit, int _texUnitID, int _value, GLenum _texTarget)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glActiveTexture(_texUnit);
        glBindTexture(_texTarget, _value);
        glUniform1i(loc, _texUnitID);
    }
}


void Shader::SetVec3(const char* _name, glm::vec3 _value)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glUniform3fv(loc, 1, &_value[0]);
    }
}

void Shader::SetMat4(const char* _name, glm::mat4 _value)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &_value[0][0]);
    }
}

void Shader::SetInt(const char* _name, int _value)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glUniform1i(loc, _value);
    }
}

void Shader::SetBool(const char* _name, bool _value)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glUniform1i(loc, _value ? 1 : 0);
    }
}

void Shader::LoadAttributes()
{
    m_attrVertices = glGetAttribLocation(m_programID, "vertices");
    m_attrWVP = glGetUniformLocation(m_programID, "WVP");
    m_attrColors = glGetAttribLocation(m_programID, "colors");
    m_attrNormals = glGetAttribLocation(m_programID, "normals");
    m_attrTexCoords = glGetAttribLocation(m_programID, "texCoords");
    m_attrTangents = glGetAttribLocation(m_programID, "tangents");
    m_attrBitangents = glGetAttribLocation(m_programID, "bitangents");
    m_attrInstanceMatrix = glGetAttribLocation(m_programID, "instanceMatrix");
}

void Shader::EvaluateShader(int _infoLength, GLuint _id)
{
    if (_infoLength > 0)
    {
        std::vector<char> errorMessage(_infoLength + 1);
        glGetShaderInfoLog(_id, _infoLength, nullptr, &errorMessage[0]);
        M_ASSERT(0, ("%s\n", &errorMessage[0]));
    }
}

GLuint Shader::LoadShaderFile(const char* _filePath, GLenum _type)
{
    GLuint shaderID = glCreateShader(_type);

    std::string shaderCode;
    std::ifstream shaderStream(_filePath, std::ios::in);
    M_ASSERT(shaderStream.is_open(),
             ("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", _filePath));
    std::string Line = "";
    while (getline(shaderStream, Line))
    {
        shaderCode += "\n" + Line;
    }
    shaderStream.close();

    const char* sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, nullptr);
    glCompileShader(shaderID);

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &m_result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &m_infoLogLength);
    EvaluateShader(m_infoLogLength, shaderID);
    glAttachShader(m_programID, shaderID);

    return shaderID;
}

void Shader::CreateShaderProgram(const char* _vertexFilePath, const char* _fragmentFilePath)
{
    m_programID = glCreateProgram();
    GLuint vertexShaderID = LoadShaderFile(_vertexFilePath, GL_VERTEX_SHADER);
    GLuint fragmentShaderID = LoadShaderFile(_fragmentFilePath, GL_FRAGMENT_SHADER);
    glLinkProgram(m_programID);

    glGetProgramiv(m_programID, GL_LINK_STATUS, &m_result);
    glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &m_infoLogLength);
    EvaluateShader(m_infoLogLength, m_programID);

    glDetachShader(m_programID, vertexShaderID);
    glDetachShader(m_programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

void Shader::LoadShaders(const char* _vertexFilePath, const char* _fragmentFilePath)
{
    CreateShaderProgram(_vertexFilePath, _fragmentFilePath);
    LoadAttributes();
}

void Shader::SetFloat(const char* _name, float _value)
{
    GLint loc = glGetUniformLocation(m_programID, _name);
    if (loc != -1)
    {
        glUniform1f(loc, _value);
    }
}
