#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

class VertexArray
{
private:
    unsigned int m_RendererID;
public:
    VertexArray();
    ~VertexArray();

    void AddBuffer(VertexBuffer& vb, const VertexBufferLayout& layout);

    void Bind() const;
    void Unbind() const;
};
