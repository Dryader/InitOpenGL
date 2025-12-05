#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
{
    m_width = 0;
    m_height = 0;
    m_channels = 0;
    m_texture = 0;
}

void Texture::Cleanup()
{
    glDeleteTextures(1, &m_texture);
}

void Texture::LoadTexture(string _fileName)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    GLubyte* data = stbi_load(_fileName.c_str(), &m_width, &m_height, &m_channels, 0);

    if (data == nullptr && _fileName != "Assets/Textures/Wood.jpg")
    {
        OutputDebugStringA(("Failed to load texture: " + _fileName + ", trying fallback Wood.jpg\n").c_str());
        data = stbi_load("Assets/Textures/Wood.jpg", &m_width, &m_height, &m_channels, 0);
    }

    if (data == nullptr)
    {
        OutputDebugStringA("Failed to load fallback texture, creating white texture\n");
        m_width = 2;
        m_height = 2;
        m_channels = 3;
        GLubyte white[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, white);
        glGenerateMipmap(GL_TEXTURE_2D);
        return;
    }

    if (EndsWith(_fileName, ".png"))
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }


    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}


void Texture::LoadCubeMap(vector<std::string> _faces)
{
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        int w, h, channels;
        GLubyte* data = stbi_load(_faces[i].c_str(), &w, &h, &channels, 0);
        if (data == nullptr)
        {
            unsigned char fallback[4] = {255, 0, 255, 255};
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallback);
            continue;
        }
        GLenum format = channels == 4 ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

bool Texture::EndsWith(const std::string& _str, const std::string& _suffix)
{
    if (_str.size() < _suffix.size()) return false;
    size_t start = _str.size() - _suffix.size();
    for (size_t i = 0; i < _suffix.size(); ++i)
    {
        char a = _str[start + i];
        char b = _suffix[i];
        if (a >= 'A' && a <= 'Z') a = static_cast<char>(a - 'A' + 'a');
        if (b >= 'A' && b <= 'Z') b = static_cast<char>(b - 'A' + 'a');
        if (a != b) return false;
    }
    return true;
}
