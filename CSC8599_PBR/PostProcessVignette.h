#pragma once
#include "PostProcessEffect.h"

class PostProcessVignette : public PostProcessEffect
{
public:
	PostProcessVignette(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessVignette();

protected:
	virtual void OnResize(const unsigned int& newSizeX, const unsigned int& newSizeY) override;
	virtual bool InitShaders() override;

public:
	virtual const unsigned int GetProcessedTexture() const override;
	virtual void Render(const unsigned int& sourceTextureID, const unsigned int& depthTextureID) override;
	virtual void OnImGuiRender() override;

private:
	float m_Amount;
	float m_FallOff;

	std::shared_ptr<Shader> m_VignetteShader;
	std::shared_ptr<Shader> m_VignetteFinalShader;

	FrameBuffer m_VignetteFBO;
	FrameBuffer m_FinalFBO;
};