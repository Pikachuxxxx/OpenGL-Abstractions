#pragma once

#include <iostream>
#include <string>
#include <vector>
// GLAD
#include <glad/glad.h>
// SOIL - Simple openGL Image Loading Library
#include <SOIL/SOIL.h>

class CubeMap
{
private:
    std::vector<const char*> m_CubemapTextures;
    GLuint m_TID;
public:
    CubeMap(std::vector<const char*> cubemapTextures);
    ~CubeMap();

    void Bind();
    void Unbind();

    inline GLuint getCubeMapTextureID() const { return m_TID; }
private:
    GLuint loadTexture();
};
