#pragma once
#include "PostProcessEffect.h"

class PostProcessSSAO : public PostProcessEffect
{
public:
	PostProcessSSAO(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessSSAO();

protected:
	virtual bool InitShaders() override;

public:
	virtual const unsigned int GetProcessedTexture() const override;
	virtual void Render(const unsigned int& sourceTextureID) override;
	virtual void OnImGuiRender() override;

private:
	std::shared_ptr<Shader> m_PostSSAOShader;

	FrameBuffer m_LastFBO;
};