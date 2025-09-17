#ifndef SHADER_H
#define SHADER_H

#include "..\OpenGL\StandardIncludes.h"

class Shader
{
public:
    // Constructors / Destructors
    Shader();
    virtual ~Shader() {}

    // Accessors
    GLuint GetProgramID() { return m_programID; }
    GLuint GetAttrVertices() { return m_attrVertices; }

    // Methods
    void LoadShaders(const char* _vertexFilePath, const char* _fragmentFilePath);
    void Cleanup();
    GLuint GetAttrWVP() {return m_attrWVP;}

private:
    // Methods
    void CreateShaderProgram(const char* _vertexFilePath, const char* _fragmentFilePath);
    GLuint LoadShaderFile(const char* _filePath, GLenum _type);
    void LoadAttributes();
    void EvaluateShader(int _infoLength, GLuint _id);

    // Members
    GLuint m_programID; // ID of out shader program
    GLuint m_attrVertices; // Handle for the attribute vertex buffer
    GLint m_result = GL_FALSE;
    int m_infoLogLength;
    GLuint m_attrWVP; 
};

#endif //SHADER_H
