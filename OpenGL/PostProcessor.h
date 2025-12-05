#ifndef POSTPROCESSOR_H
#define POSTPROCESSOR_H
#include "StandardIncludes.h"
#include "Texture.h"

class Shader;

class PostProcessor
{
public:
    PostProcessor();
    virtual ~PostProcessor();

    void Create(Shader* _postShader);
    void Cleanup();
    void Start();
    void End();

private:
    GLuint m_framebuffer;
    GLuint m_textureColorbuffer;
    GLuint m_renderBufferObject;
    GLuint m_vertexBuffer;
    Shader* m_postShader;

    void CreateVertices();
    void CreateBuffers();
    void BindVertices();
};
#endif
