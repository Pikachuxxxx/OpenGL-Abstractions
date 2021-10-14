#pragma once

#include <iostream>
#include <string>
// GLAD
#include <glad/glad.h>
#include "glassert.h"

class HDREnvironmentMap
{
public:
    GLuint m_TID;
private:
    unsigned int m_SlotID;
    int m_Width;
    int m_Height;
    int m_BPP;
    std::string m_FilePath;
public:
    HDREnvironmentMap(std::string path, unsigned int slotID);
    ~HDREnvironmentMap();

    void Bind();
    void Unbind();

    inline int getWidth() const { return m_Width; }
    inline int getHeight() const { return m_Height; }
    inline GLuint getTexture() const { return m_TID; }
private:
    GLuint loadTexture();
};
