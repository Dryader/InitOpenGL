#ifndef FONTS_H
#define FONTS_H

#include "StandardIncludes.h"

class Shader;

struct Character
{
    unsigned int TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    unsigned int Advance;
};

class Fonts
{
public:
    Fonts();

    void Create(Shader* _shader, string _name, FT_UInt _size);
    void Cleanup();
    void RenderText(std::string _text, float _x, float _y, float _scale, glm::vec3 _color);

private:
    Shader* m_shader;
    FT_Library m_library;
    FT_Face m_face;
    map<char, Character> m_characters;
    GLuint m_vertexBuffer;
    glm::mat4 m_orthoProj;

    void Initialize(string _name, FT_UInt _size);
    void CreateCharacters();
    void AllocateBuffers();
};

#endif
