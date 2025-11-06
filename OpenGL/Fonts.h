#ifndef FONTS_H
#define FONTS_H

#include "StandardIncludes.h"

class Shader;

struct Character {
    unsigned int TextureID;     // ID handle of the glyph texture
    glm::ivec2   Size;          // Size of glyph
    glm::ivec2   Bearing;       // Offset from baseline to left/top of glyph
    unsigned int Advance;       // Offset to advance to next glyph
};

class Fonts
{
public:
    // Constructors / Destructors
    Fonts();

    // Methods
    void Create(Shader* _shader, string _name, FT_UInt _size);
    void RenderText(std::string _text, float _x, float _y, float _scale, glm::vec3 _color);

private:
    // Members
    Shader* m_shader;
    FT_Library m_library;
    FT_Face m_face;
    map<char, Character> m_characters;
    GLuint m_vertexBuffer;
    glm::mat4 m_orthoProj;

    // Methods
    void Initialize(string _name, FT_UInt _size);
    void CreateCharacters();
    void AllocateBuffers();
};

#endif //FONTS_H
