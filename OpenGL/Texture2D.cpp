#include "Texture2D.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture2D::Texture2D(const char* path, int slotID)
    : m_SlotID(slotID), m_FilePath(path)
{
    m_TID = loadTexture();
}

Texture2D::~Texture2D() {}

GLuint Texture2D::loadTexture()
{
    unsigned char* image = stbi_load(m_FilePath, &m_Width, &m_Height, &m_BPP, 0);
    if (!image)
        std::cout << "Failed to load texture : " << m_FilePath << std::endl;
    else {
        std::cout << "Image details : " << std::endl;
        std::cout << "\tFile path : " << m_FilePath << std::endl;
        std::cout << "\tDimensions : (" << m_Width << ", " << m_Height << ")" << std::endl;
        std::cout << "\tBits per pixel : " << m_BPP << std::endl;
    }

    GLint imageFormat;
    switch (m_BPP) {
        case 1:
            imageFormat = GL_RED;
            break;
        case 3:
            imageFormat = GL_RGB;
            break;
        case 4:
            imageFormat = GL_RGBA;
            break;
    }

    GLuint texture;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, m_Width, m_Height, 0, imageFormat, GL_UNSIGNED_BYTE, image));
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}
void Texture2D::Bind()
{
    glActiveTexture(GL_TEXTURE0 + m_SlotID);
    glBindTexture(GL_TEXTURE_2D, m_TID);
}

void Texture2D::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
