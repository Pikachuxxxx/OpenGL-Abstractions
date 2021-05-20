#pragma once

// GLAD
#include <glad/glad.h>

class RenderBuffer
{
public:
	GLuint m_BufferID;
public:
	RenderBuffer(int Width, int Height);
	~RenderBuffer();

	void Bind() const;
	void Unbind() const;
};
