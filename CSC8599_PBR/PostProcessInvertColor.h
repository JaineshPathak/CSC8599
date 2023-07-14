#pragma once
#include "PostProcessEffect.h"

class PostProcessInvertColor : public PostProcessEffect
{
public:
	PostProcessInvertColor(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessInvertColor();

	virtual void OnResize(const unsigned int& newSizeX, const unsigned int& newSizeY) override;

protected:
	virtual bool InitShaders() override;

public:
	virtual const unsigned int GetProcessedTexture() const override;
	virtual void Render(const unsigned int& sourceTextureID, const unsigned int& depthTextureID) override;
	virtual void OnImGuiRender() override;

private:
	std::shared_ptr<Shader> m_PostInvertShader;

	FrameBuffer m_FinalFBO;
};