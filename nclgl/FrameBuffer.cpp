#include "FrameBuffer.h"
#include "Texture.h"
#include <iostream>

FrameBuffer::FrameBuffer() :
	m_ProgramID(0),
	m_Width(0), m_Height(0), 
	m_NumColorAttachments(0),
	m_ColorAttachmentsInternalFormat(GL_RGBA8), m_ColorAttachmentsNormalFormat(GL_RGBA),
	m_ColorAttachmentMinFilter(GL_LINEAR), m_ColorAttachmentMagFilter(GL_LINEAR), m_ColorAttachmentWrapMode(GL_CLAMP_TO_EDGE),
	m_ColorAttachmentMipMapsEnabled(true),
	m_ColorAttachmentsType(GL_UNSIGNED_BYTE)
{
}

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& numColorAttachments) :
	m_Width(sizeX), m_Height(sizeY), 
	m_NumColorAttachments(numColorAttachments), 
	m_ColorAttachmentsInternalFormat(GL_RGBA8), m_ColorAttachmentsNormalFormat(GL_RGBA),
	m_ColorAttachmentMinFilter(GL_LINEAR), m_ColorAttachmentMagFilter(GL_LINEAR), m_ColorAttachmentWrapMode(GL_CLAMP_TO_EDGE),
	m_ColorAttachmentMipMapsEnabled(true),
	m_ColorAttachmentsType(GL_UNSIGNED_BYTE)
{
	Invalidate();
}

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& colorAttachmentInternalFormat, const int& colorAttachmentNormalFormat, const int& colorAttachmentType, const int& numColorAttachments) :
	m_Width(sizeX), m_Height(sizeY), 
	m_NumColorAttachments(numColorAttachments),
	m_ColorAttachmentsInternalFormat(colorAttachmentInternalFormat), m_ColorAttachmentsNormalFormat(colorAttachmentNormalFormat),
	m_ColorAttachmentMinFilter(GL_LINEAR), m_ColorAttachmentMagFilter(GL_LINEAR), m_ColorAttachmentWrapMode(GL_CLAMP_TO_EDGE),
	m_ColorAttachmentMipMapsEnabled(true),
	m_ColorAttachmentsType(colorAttachmentType)
{
	Invalidate();
}

FrameBuffer::FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& colorAttachmentInternalFormat, const int& colorAttachmentNormalFormat, const int& colorAttachmentType, const unsigned int& minFilter, const unsigned int& magFilter, const unsigned int& wrapMode, const int& numColorAttachments, const bool& allowColorAttachmentMipMaps) :
	m_Width(sizeX), m_Height(sizeY), 
	m_NumColorAttachments(numColorAttachments),
	m_ColorAttachmentsInternalFormat(colorAttachmentInternalFormat), m_ColorAttachmentsNormalFormat(colorAttachmentNormalFormat),
	m_ColorAttachmentMinFilter(minFilter), m_ColorAttachmentMagFilter(magFilter), m_ColorAttachmentWrapMode(wrapMode),
	m_ColorAttachmentMipMapsEnabled(allowColorAttachmentMipMaps),
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
	
	if (m_NumColorAttachments > 0)
	{
		for (int i = 0; i < m_NumColorAttachments; i++)
			AddNewColorAttachment();
	}

	/*
	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT_24_8, nullptr);
	*/
	m_DepthAttachmentTex = std::shared_ptr<Texture>(new Texture(m_Width, m_Height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false));
	AttachExistingDepthAttachment(m_DepthAttachmentTex->GetID(), GL_DEPTH_ATTACHMENT, false);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentTex->GetID(), 0);

	std::vector<unsigned int> attachmentsV;
	if (m_NumColorAttachments > 1)
	{
		for (int i = 0; i < m_NumColorAttachments; i++)
			attachmentsV.emplace_back(GL_COLOR_ATTACHMENT0 + i);

		glDrawBuffers(m_NumColorAttachments, &attachmentsV[0]);
	}
	else
	{
		attachmentsV.emplace_back(GL_COLOR_ATTACHMENT0);
		glDrawBuffers(1, &attachmentsV[0]);
	}

	if (m_NumColorAttachments <= 0)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "Frame Buffer is Incomplete! ID: " << m_ProgramID << std::endl;	

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
	if ((int)m_ColorAttachedTextures.size() > 0)
	{
		for (int i = 0; i < (int)m_ColorAttachedTextures.size(); i++)
			m_ColorAttachedTextures[i].reset();
	}	
	m_ColorAttachedTextures.clear();

	//glDeleteTextures(1, &m_ColorAttachmentTex);
	//glDeleteTextures(1, &m_DepthAttachmentTex);
	//m_DepthAttachmentTex = 0;
	m_DepthAttachmentTex.reset();

	glDeleteFramebuffers(1, &m_ProgramID);
	m_ProgramID = 0;
}

void FrameBuffer::Resize(const unsigned int& new_width, const unsigned int& new_height)
{
	m_Width = new_width;
	m_Height = new_height;	
	
	Invalidate();
}

void FrameBuffer::AddNewColorAttachment()
{
	std::shared_ptr<Texture> newColorTex = std::shared_ptr<Texture>(new Texture(m_Width, m_Height, m_ColorAttachmentsInternalFormat, m_ColorAttachmentsNormalFormat, m_ColorAttachmentsType, m_ColorAttachmentMinFilter, m_ColorAttachmentMagFilter, m_ColorAttachmentWrapMode, m_ColorAttachmentMipMapsEnabled));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + (int)m_ColorAttachedTextures.size(), GL_TEXTURE_2D, newColorTex->GetID(), 0);
	m_ColorAttachedTextures.emplace_back(newColorTex);
}

void FrameBuffer::AttachExistingColorAttachment(const unsigned int& texID, const int& attachSlot)
{
	if (m_ProgramID <= 0)
	{
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;
		std::cerr << "Frame Buffer ERROR: Object does not exist! Function: " << __FUNCTION__ << std::endl;
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;

		return;
	}

	if (attachSlot < 0)
	{
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;
		std::cerr << "Frame Buffer ERROR: Attachment Slot less than zero is invalid! Function: " << __FUNCTION__ << std::endl;
		std::cerr << "Frame Buffer Object ID: " << m_ProgramID << std::endl;
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;

		return;
	}

	Bind();
	glBindTexture(GL_TEXTURE_2D, texID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachSlot, GL_TEXTURE_2D, texID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	Unbind();
}

void FrameBuffer::AttachExistingDepthAttachment(const unsigned int& texID, const unsigned int& depthComponentType, const bool& doBinding)
{
	if (m_ProgramID <= 0)
	{
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;
		std::cerr << "Frame Buffer ERROR: Object does not exist! Function: " << __FUNCTION__ << std::endl;
		std::cerr << "-----------------------------------------------------------------------------" << std::endl;

		return;
	}

	if(doBinding) Bind();
	glBindTexture(GL_TEXTURE_2D, texID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, depthComponentType, GL_TEXTURE_2D, texID, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	if(doBinding) Unbind();
}

void FrameBuffer::RemoveDepthAttachment()
{
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	Unbind();
}

const unsigned int FrameBuffer::GetColorAttachmentTex(const int& index) const
{
	if (index < 0 || index >= (int)m_ColorAttachedTextures.size())
	{
		std::cerr << "ERROR: Frame Buffer with ID: " << m_ProgramID << ". Message: Invalid index given. Max Color Attachments Size: " << (int)m_ColorAttachedTextures.size() << std::endl;
		return -1;
	}

	return m_ColorAttachedTextures[index]->GetID();
}

const unsigned int FrameBuffer::GetDepthAttachmentTex() const
{
	return m_DepthAttachmentTex->GetID();
}
