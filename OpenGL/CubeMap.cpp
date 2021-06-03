#include "CubeMap.h"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

CubeMap::CubeMap()
{
    // m_TID = loadTexture();
}

CubeMap::~CubeMap() {}

GLint CubeMap::loadTextures(std::vector<const char*> cubemapTextures)
{
    m_CubemapTextures = cubemapTextures;

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, bpp;
    unsigned char* image;
    for (int tex = 0; tex < m_CubemapTextures.size(); tex++)
    {
        image = stbi_load(m_CubemapTextures[tex], &width, &height, &bpp, 0);
        if (!image)
            std::cout << "Failed to load texture : " << m_CubemapTextures[tex] << std::endl;
        else{
            std::cout << "Image details : " << std::endl;
            std::cout << "\tFile path : " << m_CubemapTextures[tex] << " ID : " << tex << std::endl;
            std::cout << "\tDimensions : (" << width << ", " << height << ")" << std::endl;
            std::cout << "\tBits per pixel : " << bpp << std::endl;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    m_TID = texture;

    return texture;
}
void CubeMap::Bind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TID);
}

void CubeMap::Unbind()
{
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
