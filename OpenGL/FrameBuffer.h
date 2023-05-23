#pragma once

// GLAD
#include <glad/glad.h>
#include "RenderBuffer.h"

#include <vector>

class FrameBuffer
{
public:
	GLuint						m_BufferID;
	RenderBuffer*				m_RBO;
	std::vector<unsigned int>	m_Attachments;
public:
	FrameBuffer();
	~FrameBuffer();

	void Create(int Width, int Height);

	void Bind() const;
	void Unbind() const;

	/* idx must be greater than 1 as we have a default color attachment at 0 */
	void AddAttachment(int idx, int internalFormat, int format, int Width, int Height);

	inline unsigned int getAttachmentAt(int idx) { return m_Attachments[idx]; }
	inline unsigned int getDepthAttachment() { return m_RBO->m_BufferID; }
};
