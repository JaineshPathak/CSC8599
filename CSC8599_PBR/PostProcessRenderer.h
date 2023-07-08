#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"
#include <nclgl/FrameBufferBloom.h>

class Shader;
class Texture;
class PostProcessRenderer : public IImguiItem
{
public:
	PostProcessRenderer() = delete;
	PostProcessRenderer(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessRenderer();

	const bool IsInitialized() const { return m_IsInitialized; }
	const bool IsBloomEnabled() const { return m_EnableBloom; }

protected:
	bool InitShaders();	

	void RenderBrightColors(unsigned int srcTexture);
	void RenderDownSamples(unsigned int srcTexture);
	void RenderUpSamples();
	void RenderBloomTexture(unsigned int srcTexture);

public:
	unsigned int GetBloomTexture(int index = 0);
	unsigned int GetFinalTexture();

	void Render(unsigned int srcTexture);
	virtual void OnImGuiRender() override;

protected:
	bool m_IsInitialized;

	float m_WidthF, m_HeightF;
	int m_WidthI, m_HeightI;
	Vector2 m_SrcViewportSize;

	bool m_EnableBloom;
	float m_BloomFilterRadius;
	float m_BrightnessThreshold;
	float m_BloomStrength;

	std::shared_ptr<Shader> m_PostBloomBrightenShader;
	std::shared_ptr<Shader> m_PostBloomDownSampleShader;
	std::shared_ptr<Shader> m_PostBloomUpSampleShader;
	std::shared_ptr<Shader> m_PostFinalShader;

	FrameBufferBloom m_BloomFBO;
	FrameBuffer m_FinalFBO;
	FrameBuffer m_BrightenFBO;
};