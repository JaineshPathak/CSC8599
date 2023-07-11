#pragma once
#include "PostProcessEffect.h"
#include <nclgl/FrameBufferBloom.h>

class PostProcessBloom : public PostProcessEffect
{
public:
	PostProcessBloom(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessBloom();

protected:
	virtual bool InitShaders() override;
	virtual bool InitTextures() override;

private:
	void RenderBrightColors(unsigned int srcTexture);
	void RenderDownSamples(unsigned int srcTexture);
	void RenderUpSamples();
	void RenderBloomTexture(unsigned int srcTexture);

	void OnIterationsChanged();

	unsigned int GetBloomTexture(int index = 0);

public:
	virtual const unsigned int GetProcessedTexture() const override;
	virtual void Render(const unsigned int& sourceTextureID) override;
	virtual void OnImGuiRender() override;

private:
	int m_BloomIterations;
	bool m_EnableBloomDebug;
	float m_BloomFilterRadius;

	float m_BrightnessThreshold;
	float m_BrightnessSoftThreshold;

	float m_BloomStrength;

	Vector4 m_BloomTint;
	float m_BloomTintStrength;

	bool m_EnableDirtMask;
	float m_DirtMaskStrength;

	std::shared_ptr<Texture> m_DirtMaskTexture;
	std::shared_ptr<Shader> m_PostBloomBrightenShader;
	std::shared_ptr<Shader> m_PostBloomDownSampleShader;
	std::shared_ptr<Shader> m_PostBloomUpSampleShader;
	std::shared_ptr<Shader> m_PostBloomFinalShader;

	FrameBufferBloom m_BloomFBO;
	FrameBuffer m_LastFBO;
	FrameBuffer m_BrightenFBO;
};