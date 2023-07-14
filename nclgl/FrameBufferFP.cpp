#include "FrameBufferFP.h"
#include <iostream>

void FrameBufferFP::Invalidate()
{
	if (m_ProgramID)
	{
		Destroy();
	}

	glCreateFramebuffers(1, &m_ProgramID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_ProgramID);

	/*glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentTex, 0);*/

	/*
	glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentTex);
	glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Width, m_Height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentTex, 0);
	*/

	m_DepthAttachmentTex = std::shared_ptr<Texture>(new Texture(m_Width, m_Height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_EDGE, false));
	AttachExistingDepthAttachment(m_DepthAttachmentTex->GetID(), GL_DEPTH_ATTACHMENT, false);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame Buffer is Incomplete! ID: " << m_ProgramID << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
