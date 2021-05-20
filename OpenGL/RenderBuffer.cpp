#include "RenderBuffer.h"

RenderBuffer::RenderBuffer(int Width, int Height)
{
	glGenRenderbuffers(1, &m_BufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_BufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

RenderBuffer::~RenderBuffer()
{
	glDeleteRenderbuffers(1, &m_BufferID);
}

void RenderBuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_BufferID);
}

void RenderBuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
