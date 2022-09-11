#pragma once

#include <iostream>
#include <string>
// GLAD
#include <glad/glad.h>
#include "glassert.h"

#include <glm/glm.hpp>

// TOOD: Add width, height, depth dimensions
class Texture3D
{
public:
    GLuint m_TID;
    int m_SlotID;
    int m_Dimensions;   // TODO: Make this a glm::vec3
    const char* m_FilePath;
public:
    Texture3D(void* data, int dimensions, int slotID = 0);
    ~Texture3D();

    void Bind();
    void Unbind();

    void Clear(glm::vec4 clearColor);
    void UpdateData(void* data);

    static void fill_pixel(GLfloat* data, int x, int y, int z, int w, int h, int d);
    static void fill_corners(GLfloat* data, int w, int h, int d);

    inline GLuint getTexture() const { return m_TID; }
};
