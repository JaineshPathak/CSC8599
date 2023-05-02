#pragma once

#include <glad/glad.h>

class FrameBuffer
{
public:
	FrameBuffer(int sizeX, int sizeY);
	~FrameBuffer();

	void Invalidate();

	void Bind();
	void Unbind();

	unsigned int GetColorAttachmentTex() const { return m_ColorAttachmentTex; }
	unsigned int GetDepthAttachmentTex() const { return m_DepthAttachmentTex; }

	const int GetWidth() const { return m_Width; }
	const int GetHeight() const { return m_Height; }

protected:
	unsigned int m_RendererID;
	unsigned int m_ColorAttachmentTex = 0, m_DepthAttachmentTex = 0;

	unsigned int m_Width, m_Height;
};