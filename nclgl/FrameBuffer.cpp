#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY) :
	m_Width(sizeX), m_Height(sizeY)
{
	Invalidate();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &m_ProgramID);
	glDeleteTextures(1, &m_ColorAttachmentTex);
	glDeleteTextures(1, &m_DepthAttachmentTex);
}

void FrameBuffer::Invalidate()
{
	if (m_ProgramID)
	{
		glDeleteFramebuffers(1, &m_ProgramID);
		glDeleteTextures(1, &m_ColorAttachmentTex);
		glDeleteTextures(1, &m_DepthAttachmentTex);
	}

	glCreateFramebuffers(1, &m_ProgramID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ProgramID);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentTex, 0);

	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame Buffer is Incomplete! ID: " << m_ProgramID << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind()
{
	glViewport(0, 0, m_Width, m_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ProgramID);
}

void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Resize(const unsigned int& new_width, const unsigned int& new_height)
{
	m_Width = new_width;
	m_Height = new_height;	
	
	Invalidate();
}
