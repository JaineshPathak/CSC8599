#include "FrameBuffer.h"
#include "Texture.h"
#include <iostream>

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& numColorAttachments) :
	m_Width(sizeX), m_Height(sizeY), m_NumColorAttachments(numColorAttachments), 
	m_ColorAttachmentsInternalFormat(GL_RGBA8), m_ColorAttachmentsNormalFormat(GL_RGBA),
	m_ColorAttachmentsType(GL_UNSIGNED_BYTE)
{
	Invalidate();
}

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& colorAttachmentInternalFormat, const int& colorAttachmentNormalFormat, const int& colorAttachmentType, const int& numColorAttachments) :
	m_Width(sizeX), m_Height(sizeY), m_NumColorAttachments(numColorAttachments),
	m_ColorAttachmentsInternalFormat(colorAttachmentInternalFormat), m_ColorAttachmentsNormalFormat(colorAttachmentNormalFormat),
	m_ColorAttachmentsType(colorAttachmentType)
{
	Invalidate();
}

FrameBuffer::~FrameBuffer()
{
	Destroy();
}

void FrameBuffer::Invalidate()
{
	if (m_ProgramID)	
		Destroy();	

	glCreateFramebuffers(1, &m_ProgramID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ProgramID);

	/*glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentTex, 0);*/
	for (int i = 0; i < m_NumColorAttachments; i++)
		AddNewColorAttachment();

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

void FrameBuffer::Destroy()
{
	glDeleteFramebuffers(1, &m_ProgramID);
	for (int i = 0; i < m_NumColorAttachments; i++)
		m_ColorAttachedTextures[i].reset();
	//glDeleteTextures(1, &m_ColorAttachmentTex);
	glDeleteTextures(1, &m_DepthAttachmentTex);

	m_ColorAttachedTextures.clear();
}

void FrameBuffer::Resize(const unsigned int& new_width, const unsigned int& new_height)
{
	m_Width = new_width;
	m_Height = new_height;	
	
	Invalidate();
}

void FrameBuffer::AddNewColorAttachment()
{
	std::shared_ptr<Texture> newColorTex = std::shared_ptr<Texture>(new Texture(m_Width, m_Height, m_ColorAttachmentsInternalFormat, m_ColorAttachmentsNormalFormat, m_ColorAttachmentsType));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)m_ColorAttachedTextures.size(), GL_TEXTURE_2D, newColorTex->GetID(), 0);
	m_ColorAttachedTextures.emplace_back(newColorTex);
}

const unsigned int FrameBuffer::GetColorAttachmentTex(const int& index) const
{
	return m_ColorAttachedTextures[index]->GetID();
}
