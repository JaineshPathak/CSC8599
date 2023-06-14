#pragma once

#include <glad/glad.h>

class FrameBuffer
{
public:
	FrameBuffer() = delete;
	FrameBuffer(const unsigned int& sizeX, const unsigned int& sizeY);
	virtual ~FrameBuffer();

protected:
	virtual void Invalidate();

public:
	virtual void Bind();
	virtual void Unbind();
	void Resize(const unsigned int& new_width, const unsigned int& new_height);

	const unsigned int GetID() const { return m_ProgramID; }
	const unsigned int GetColorAttachmentTex() const { return m_ColorAttachmentTex; }
	const unsigned int GetDepthAttachmentTex() const { return m_DepthAttachmentTex; }

	const unsigned int GetWidth() const { return m_Width; }
	const unsigned int GetHeight() const { return m_Height; }

protected:
	unsigned int m_ProgramID;
	unsigned int m_ColorAttachmentTex = 0, m_DepthAttachmentTex = 0;

	unsigned int m_Width, m_Height;
};