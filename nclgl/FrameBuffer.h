#pragma once

#include <glad/glad.h>
#include <vector>
#include <memory>

class Texture;
class FrameBuffer
{
public:
	FrameBuffer() = delete;
	FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& numColorAttachments = 1);
	FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY, const int& colorAttachmentInternalFormat, const int& colorAttachmentNormalFormat, const int& colorAttachmentType, const int& numColorAttachments = 1);
	virtual ~FrameBuffer();

protected:
	virtual void Invalidate();

public:
	virtual void Bind();
	virtual void Unbind();
	virtual void Destroy();
	void Resize(const unsigned int& new_width, const unsigned int& new_height);
	void AddNewColorAttachment();

	const unsigned int GetID() const { return m_ProgramID; }
	const unsigned int GetColorAttachmentTex(const int& index = 0) const;
	const unsigned int GetDepthAttachmentTex() const { return m_DepthAttachmentTex; }

	const unsigned int GetWidth() const { return m_Width; }
	const unsigned int GetHeight() const { return m_Height; }

	const int GetNumColorAttachments() const { return m_NumColorAttachments; }

protected:
	unsigned int m_ProgramID;
	//unsigned int m_ColorAttachmentTex = 0, m_DepthAttachmentTex = 0;
	std::vector<std::shared_ptr<Texture>> m_ColorAttachedTextures;
	unsigned int m_DepthAttachmentTex = 0;

	unsigned int m_Width, m_Height;
	int m_NumColorAttachments;
	int m_ColorAttachmentsInternalFormat;
	int m_ColorAttachmentsNormalFormat;
	int m_ColorAttachmentsType;	//Eg: GL_FLOAT, GL_UNSIGNED_INT
};