#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"

#include "PostProcessEffect.h"
#include "PostProcessBloom.h"
#include "PostProcessInvertColor.h"

#include <vector>

class PostProcessRenderer
{
public:
	PostProcessRenderer() = delete;
	PostProcessRenderer(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessRenderer();

	const unsigned int GetFinalTexture() const;
	const bool IsInitialized() const { return m_IsInitialized; }

	const bool IsEnabled() const { return m_IsEnabled; }
	void SetActive(const bool& status) { m_IsEnabled = status; }

	void Render(const unsigned int& srcTexture);

private:
	void FillActivePostEffects();
	void RenderActivePostEffects(const unsigned int& srcTexture);
	void RenderFinalPostEffect();
	void AddPostProcessEffect(std::shared_ptr<PostProcessEffect> effect);

protected:
	bool m_IsEnabled;
	bool m_IsInitialized;

	FrameBuffer m_FinalFBO;
	std::shared_ptr<Shader> m_PostFinalShader;

private:
	unsigned int m_FinalTextureID;
	std::vector<std::shared_ptr<PostProcessEffect>> m_PostEffects;
	std::vector<std::shared_ptr<PostProcessEffect>> m_ActivePostEffects;
};