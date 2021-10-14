#include "HDREnvironmentMap.h"

#include <stb_image.h>

HDREnvironmentMap::HDREnvironmentMap(std::string path, unsigned int slotID)
    : m_FilePath(path), m_SlotID(slotID)
{
    m_TID = loadTexture();
}

HDREnvironmentMap::~HDREnvironmentMap() {}

GLuint HDREnvironmentMap::loadTexture()
{
    stbi_set_flip_vertically_on_load(true);
    float* image = stbi_loadf(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 0);
    if (!image)
        std::cout << "Failed to load HDR cubemap : " << m_FilePath << std::endl;
    else {
        std::cout << "Image details    : "  << std::endl;
        std::cout << "\tFile path      : "  << m_FilePath << std::endl;
        std::cout << "\tDimensions     : (" << m_Width << ", " << m_Height << ")" << std::endl;
        std::cout << "\tBits per pixel : "  << m_BPP << std::endl;
    }

    GLuint texture;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, image));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

void HDREnvironmentMap::Bind()
{
    glActiveTexture(GL_TEXTURE0 + m_SlotID);
    glBindTexture(GL_TEXTURE_2D, m_TID);
}

void HDREnvironmentMap::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
