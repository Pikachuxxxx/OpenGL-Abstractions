#include "CubeMap.h"


CubeMap::CubeMap(std::vector<const char*> cubemapTextures)
    : m_CubemapTextures(cubemapTextures)
{
    m_TID = loadTexture();
}

CubeMap::~CubeMap() {}

GLuint CubeMap::loadTexture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height;
    unsigned char* image;
    for (int tex = 0; tex < m_CubemapTextures.size(); tex++)
    {
        image = stbi_load(m_CubemapTextures[tex], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + tex, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

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
