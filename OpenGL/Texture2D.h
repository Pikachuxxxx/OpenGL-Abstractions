#pragma once

#include <iostream>
#include <string>
// GLAD
#include <glad/glad.h>

class Texture2D
{
public:
    GLuint m_TID;
private:
    int m_Width;
    int m_Height;
    int m_BPP;
    const char* m_FilePath;
public:
    Texture2D(const char* path);
    ~Texture2D();

    void Bind();
    void Unbind();

    inline int getWidth() const { return m_Width; }
    inline int getHeight() const { return m_Height; }
    inline GLuint getTexture() const { return m_TID; }
private:
    GLuint loadTexture();
};
