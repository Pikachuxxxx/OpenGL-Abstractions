#pragma once

#include <iostream>
#include <string>
#include <vector>
// GLAD
#include <glad/glad.h>

class CubeMap
{
private:
    std::vector<const char*> m_CubemapTextures;
    GLuint m_TID;
public:
    CubeMap();
    ~CubeMap();

    void Bind();
    void Unbind();

    inline GLuint getCubeMapTextureID() const { return m_TID; }
public:
    GLint loadTextures(std::vector<const char*> cubemapTextures);
};
