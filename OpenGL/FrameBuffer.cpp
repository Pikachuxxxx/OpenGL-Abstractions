#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer()
{
	
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_BufferID);
}

void FrameBuffer::Create(int Width, int Height)
{
    glGenFramebuffers(1, &m_BufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);

    // Add the other attachments
    std::vector<unsigned int> attachments{};
    for (size_t i = 0; i < m_Attachments.size(); i++) {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_Attachments[i], 0);
    }

    glDrawBuffers(m_Attachments.size(), attachments.data());

    m_RBO = new RenderBuffer(Width, Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO->m_BufferID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_BufferID);
}

void FrameBuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::AddAttachment(int idx, int internalFormat, int format, int Width, int Height)
{

    unsigned int rt;
    glGenTextures(1, &rt);
    glBindTexture(GL_TEXTURE_2D, rt);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, Width, Height, 0, format, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    m_Attachments.push_back(rt);
}
