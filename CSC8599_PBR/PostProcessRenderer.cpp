#include "PostProcessRenderer.h"
#include "Renderer.h"

PostProcessRenderer::PostProcessRenderer(const unsigned int& sizeX, const unsigned int& sizeY) :
	m_IsEnabled(true),
	m_Width(sizeX),
	m_Height(sizeY)
{
	m_PostFinalShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostFinalFrag.glsl"));
	if (!m_PostFinalShader->LoadSuccess()) return;

	m_FinalFBO.~FrameBuffer();
	new(&m_FinalFBO) FrameBuffer(m_Width, m_Height, GL_RGB16F, GL_RGB, GL_FLOAT, 1);

	AddPostProcessEffect(std::shared_ptr<PostProcessEffect>(new PostProcessSSAO(m_Width, m_Height)));
	AddPostProcessEffect(std::shared_ptr<PostProcessEffect>(new PostProcessBloom(m_Width, m_Height)));
	AddPostProcessEffect(std::shared_ptr<PostProcessEffect>(new PostProcessInvertColor(m_Width, m_Height)));

	m_IsInitialized = true;
}

PostProcessRenderer::~PostProcessRenderer()
{
	m_IsInitialized = false;
}

const unsigned int PostProcessRenderer::GetFinalTexture() const
{
	//return m_FinalTextureID;
	return m_FinalFBO.GetColorAttachmentTex();
}

void PostProcessRenderer::Render(const unsigned int& srcTexture, const unsigned int& depthTextureID)
{
	if (!m_IsEnabled) return;

	if (Renderer::Get()->GetGlobalFrameBuffer()->GetWidth() != m_Width || Renderer::Get()->GetGlobalFrameBuffer()->GetHeight() != m_Height)
	{
		m_Width = Renderer::Get()->GetGlobalFrameBuffer()->GetWidth();
		m_Height = Renderer::Get()->GetGlobalFrameBuffer()->GetHeight();
		OnResize(m_Width, m_Height);
	}
	
	FillActivePostEffects();
	RenderActivePostEffects(srcTexture, depthTextureID);
	RenderFinalPostEffect();
	
	m_ActivePostEffects.clear();
}

void PostProcessRenderer::OnResize(const unsigned int& newSizeX, const unsigned int& newSizeY)
{
	m_FinalFBO.Resize(newSizeX, newSizeY);

	if ((int)m_PostEffects.size() == 0) return;

	for (int i = 0; i < (int)m_PostEffects.size(); i++)	
		m_PostEffects[i]->OnResize(newSizeX, newSizeY);
}

void PostProcessRenderer::FillActivePostEffects()
{
	if ((int)m_PostEffects.size() == 0) return;

	for (int i = 1; i < (int)m_PostEffects.size(); i++)
	{
		if (m_PostEffects[i]->IsEnabled())
			m_ActivePostEffects.emplace_back(m_PostEffects[i]);
	}
}

void PostProcessRenderer::RenderActivePostEffects(const unsigned int& srcTexture, const unsigned int& depthTextureID)
{
	if ((int)m_ActivePostEffects.size() == 0)
	{
		m_FinalTextureID = srcTexture;
		return;
	}

	for (int i = 0; i < (int)m_ActivePostEffects.size(); i++)
	{
		PostProcessEffect& effect = *m_ActivePostEffects[i];
		effect.Render(i == 0 ? srcTexture : m_ActivePostEffects[i - 1]->GetProcessedTexture(), depthTextureID);
	}

	m_FinalTextureID = m_ActivePostEffects[m_ActivePostEffects.size() - 1]->GetProcessedTexture();
}

void PostProcessRenderer::RenderFinalPostEffect()
{
	m_FinalFBO.Bind();
	m_PostFinalShader->Bind();
	m_PostFinalShader->SetInt("isPostProcessEnabled", (int)m_ActivePostEffects.size() > 0 && m_IsEnabled);
	m_PostFinalShader->SetTexture("srcTexture", Renderer::Get()->GetGlobalFrameBuffer()->GetColorAttachmentTex(0), 0);
	m_PostFinalShader->SetTexture("postProcessTexture", m_FinalTextureID, 1);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::Get()->GetQuadMesh()->Draw();

	m_PostFinalShader->UnBind();
	m_FinalFBO.Unbind();
}

void PostProcessRenderer::AddPostProcessEffect(std::shared_ptr<PostProcessEffect> effect)
{
	m_PostEffects.emplace_back(effect);
}