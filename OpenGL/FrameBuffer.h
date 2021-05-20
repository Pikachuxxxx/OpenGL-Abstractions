#pragma once


// GLAD
#include <glad/glad.h>
#include "RenderBuffer.h"

class FrameBuffer
{
private:
	GLuint		   m_BufferID;
	GLuint		   m_RenderTexture;
	RenderBuffer*  m_RBO;
public:
	FrameBuffer(int Width, int Height);
	~FrameBuffer();

	void Bind() const;
	void Unbind() const;

	inline GLuint getRenderTexture() const { return m_RenderTexture;  }
};
