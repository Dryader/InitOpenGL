#include "Fonts.h"
#include "Shader.h"
#include "WindowController.h"

Fonts::Fonts()
{
    m_shader = nullptr;
    m_library = nullptr;
    m_face = nullptr;
    m_vertexBuffer = 0;
    m_orthoProj = {};
}

void Fonts::Create(Shader* _shader, string _name, FT_UInt _size)
{
    m_shader = _shader;

    AllocateBuffers();
    Initialize(_name, _size);
}

void Fonts::Cleanup()
{
    glDeleteBuffers(1, &m_vertexBuffer);
    map<char, Character>::iterator it;
    for (it = m_characters.begin(); it != m_characters.end(); it++)
    {
        glDeleteTextures(1, &it->second.TextureID);
    }
}

void Fonts::AllocateBuffers()
{
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
}

void Fonts::Initialize(string _name, FT_UInt _size)
{
    // Build orthographic projection using current resolution
    Resolution r = WindowController::GetInstance().GetResolution();
    m_orthoProj = glm::ortho(0.0f, static_cast<float>(r.m_width), 0.0f, static_cast<float>(r.m_height));

    // Initialize the library and font face
    M_ASSERT(FT_Init_FreeType(&m_library) == false, "Could not init FreeType Library");

    // Try a few likely locations so it works regardless of working directory
    bool loaded = false;
    std::string chosenPath;
    const std::string candidates[] = {
        "External/" + _name,
        "../External/" + _name,
        "../../External/" + _name,
        "Assets/Fonts/" + _name,
        "../Assets/Fonts/" + _name
    };
    for (const auto& p : candidates)
    {
        if (FT_New_Face(m_library, p.c_str(), 0, &m_face) == 0)
        {
            loaded = true;
            chosenPath = p;
            break;
        }
    }
    M_ASSERT(loaded == true, "Failed to load font");

    // Set font height to _size (0 to auto-calculate width)
    M_ASSERT(FT_Set_Pixel_Sizes(m_face, 0, _size) == false, "Failed to set character size");

    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    CreateCharacters();

    // Free library resources
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_library);
}

void Fonts::CreateCharacters()
{
    // Load first 128 character glyphs into character textures
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        M_ASSERT(FT_Load_Char(m_face, c, FT_LOAD_RENDER) == false, "Failed to load Glyph");

        // Generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows),
            glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top),
            static_cast<unsigned int>(m_face->glyph->advance.x)
        };

        m_characters.insert(std::pair<char, Character>(c, character));
    }
}

void Fonts::RenderText(std::string _text, float _x, float _y, float _scale, glm::vec3 _color)
{
    Resolution v = WindowController::GetInstance().GetResolution();
    _y = static_cast<float>(v.m_height) - _y;
    glUseProgram(m_shader->GetProgramID()); // Use our shader
    m_shader->SetVec3("TextColor", _color);
    glUniformMatrix4fv(glGetUniformLocation(m_shader->GetProgramID(), "Projection"), 1, GL_FALSE,
                       glm::value_ptr(m_orthoProj));
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

    // Safety check: ensure vertex attribute exists
    GLint attrVertices = m_shader->GetAttrVertices();
    if (attrVertices == -1)
    {
        return; // Attribute not found in shader, exit safely
    }

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = _text.begin(); c != _text.end(); ++c)
    {
        Character ch = m_characters[*c];
        float xpos = _x + ch.Bearing.x * _scale;
        float ypos = _y - (ch.Size.y - ch.Bearing.y) * _scale;
        float w = ch.Size.x * _scale;
        float h = ch.Size.y * _scale;

        // Update vertex buffer for each character
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos, ypos, 0.0f, 1.0f},
            {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f},
            {xpos + w, ypos, 1.0f, 1.0f},
            {xpos + w, ypos + h, 1.0f, 0.0f},
        };

        // Render glyph texture over quad
        m_shader->SetTextureSampler("material.diffuseTexture", GL_TEXTURE0, 0, ch.TextureID);

        // Update content of vertex buffer memory
        glEnableVertexAttribArray(attrVertices);
        glVertexAttribPointer(attrVertices, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        // Be sure to use glBufferSubData and not glBufferData

        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        _x += (ch.Advance >> 6) * _scale;
        // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))

        glDisableVertexAttribArray(attrVertices);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
