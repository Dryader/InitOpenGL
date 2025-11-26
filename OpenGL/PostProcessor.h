#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H
#include "StandardIncludes.h"
#include "Texture.h"

class Shader;

class PostProcessor
{
public:
    // Constructors / Destructors
    PostProcessor();
    virtual ~PostProcessor();

    // Methods
    void Create(Shader* _postShader);
    void Cleanup();
    void Start();
    void End();

private:
    // Members
    GLuint m_framebuffer;
    GLuint m_textureColorbuffer;
    GLuint m_renderBufferObject;
    GLuint m_vertexBuffer; // GPU vertex buffer
    Shader* m_postShader;

    // Methods
    void CreateVertices();
    void CreateBuffers();
    void BindVertices();
};
#endif // POSTPROCESSOR_H
