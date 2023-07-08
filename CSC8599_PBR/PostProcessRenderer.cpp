#include "PostProcessRenderer.h"
#include "Renderer.h"

#include <nclgl/Shader.h>
#include <nclgl/Texture.h>
#include <nclgl/Mesh.h>

PostProcessRenderer::PostProcessRenderer(const unsigned int& sizeX, const unsigned int& sizeY) :
	m_WidthF((float)sizeX), m_HeightF((float)sizeY),
	m_WidthI(sizeX), m_HeightI(sizeY),
	m_EnableBloom(true),
	m_BloomFilterRadius(0.005f),
	m_BrightnessThreshold(0.6f),
	m_BloomStrength(1.0f)
{
	if (!InitShaders()) { m_IsInitialized = false; return; }

	const unsigned int numBloomMips = 5;
	m_BloomFBO.~FrameBufferBloom();
	new(&m_BloomFBO) FrameBufferBloom(m_WidthI, m_HeightI, numBloomMips);

	m_FinalFBO.~FrameBuffer();
	new(&m_FinalFBO) FrameBuffer(m_WidthI, m_HeightI, GL_RGB16F, GL_RGB, GL_FLOAT, 1);

	m_BrightenFBO.~FrameBuffer();
	new(&m_BrightenFBO) FrameBuffer(m_WidthI, m_HeightI, GL_RGB16F, GL_RGB, GL_FLOAT, 1);

	m_SrcViewportSize = Vector2(m_WidthF, m_HeightF);

	ImGuiRenderer::Get()->RegisterItem(this);

	m_IsInitialized = true;
}

PostProcessRenderer::~PostProcessRenderer()
{
	m_BloomFBO.Destroy();
	m_IsInitialized = false;
}

bool PostProcessRenderer::InitShaders()
{
	m_PostBloomBrightenShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostBloomBrightenFrag.glsl"));
	if (!m_PostBloomBrightenShader->LoadSuccess()) return false;

	m_PostBloomDownSampleShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostBloomDownSampleFrag.glsl"));
	if (!m_PostBloomDownSampleShader->LoadSuccess()) return false;

	m_PostBloomUpSampleShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostBloomUpSampleFrag.glsl"));
	if (!m_PostBloomUpSampleShader->LoadSuccess()) return false;

	m_PostFinalShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostFinalFrag.glsl"));
	if (!m_PostFinalShader->LoadSuccess()) return false;

	m_PostBloomDownSampleShader->Bind();
	m_PostBloomDownSampleShader->SetInt("srcTexture", 0);
	m_PostBloomDownSampleShader->UnBind();

	m_PostBloomUpSampleShader->Bind();
	m_PostBloomUpSampleShader->SetInt("srcTexture", 0);
	m_PostBloomUpSampleShader->UnBind();

	return true;
}

void PostProcessRenderer::RenderBrightColors(unsigned int srcTexture)
{
	m_BrightenFBO.Bind();
	m_PostBloomBrightenShader->Bind();
	m_PostBloomBrightenShader->SetTexture("srcTexture", srcTexture, 0);
	m_PostBloomBrightenShader->SetFloat("brightnessThreshold", m_BrightnessThreshold);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::Get()->GetQuadMesh()->Draw();

	m_PostBloomBrightenShader->UnBind();
	m_BrightenFBO.Unbind();
}

void PostProcessRenderer::RenderDownSamples(unsigned int srcTexture)
{
	const std::vector<BloomMip>& mipChain = m_BloomFBO.MipChain();

	m_PostBloomDownSampleShader->Bind();
	m_PostBloomDownSampleShader->SetVector2("srcResolution", m_SrcViewportSize);
	m_PostBloomDownSampleShader->SetTexture("srcTexture", srcTexture, 0);

	for (int i = 0; i < (int)mipChain.size(); i++)
	{
		const BloomMip& mip = mipChain[i];

		glViewport(0, 0, mip.sizeX, mip.sizeY);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);		

		Renderer::Get()->GetQuadMesh()->Draw();
		
		m_PostBloomDownSampleShader->SetVector2("srcResolution", mip.size);
		m_PostBloomDownSampleShader->SetTexture("srcTexture", mip.texture, 0);
	}

	m_PostBloomDownSampleShader->UnBind();
}

void PostProcessRenderer::RenderUpSamples()
{
	const std::vector<BloomMip>& mipChain = m_BloomFBO.MipChain();

	m_PostBloomUpSampleShader->Bind();
	m_PostBloomUpSampleShader->SetFloat("filterRadius", m_BloomFilterRadius);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	for (int i = (int)mipChain.size() - 1; i > 0; i--)
	{
		const BloomMip& mip = mipChain[i];
		const BloomMip& nextMip = mipChain[i - 1];

		m_PostBloomUpSampleShader->SetTexture("srcTexture", mip.texture, 0);

		glViewport(0, 0, nextMip.sizeX, nextMip.sizeY);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextMip.texture, 0);

		Renderer::Get()->GetQuadMesh()->Draw();
	}

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	//glDisable(GL_BLEND);

	m_PostBloomUpSampleShader->UnBind();
}

void PostProcessRenderer::RenderBloomTexture(unsigned int srcTexture)
{
	m_BloomFBO.Bind();

	RenderDownSamples(srcTexture);
	RenderUpSamples();

	m_BloomFBO.Unbind();

	glViewport(0, 0, m_WidthI, m_HeightI);
}

unsigned int PostProcessRenderer::GetBloomTexture(int index)
{
	return m_BloomFBO.MipChain()[index].texture;
}

unsigned int PostProcessRenderer::GetFinalTexture()
{
	return m_FinalFBO.GetColorAttachmentTex();
}

void PostProcessRenderer::Render(unsigned int srcTexture)
{
	if (!m_EnableBloom) return;

	RenderBrightColors(srcTexture);
	RenderBloomTexture(m_BrightenFBO.GetColorAttachmentTex());

	m_FinalFBO.Bind();
	m_PostFinalShader->Bind();
	m_PostFinalShader->SetTexture("srcTexture", Renderer::Get()->GetGlobalFrameBuffer()->GetColorAttachmentTex(0), 0);
	m_PostFinalShader->SetTexture("postProcessTexture", GetBloomTexture(), 1);
	m_PostFinalShader->SetFloat("bloomStrength", m_BloomStrength);
	
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FinalFBO.GetColorAttachmentTex(), 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::Get()->GetQuadMesh()->Draw();
	m_PostFinalShader->UnBind();
	m_FinalFBO.Unbind();

	/*glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_PostFinalShader->Bind();
	m_PostFinalShader->SetTexture("srcTexture", Renderer::Get()->GetGlobalFrameBuffer()->GetColorAttachmentTex(0), 0);
	m_PostFinalShader->SetTexture("postProcessTexture", GetBloomTexture(), 1);

	Renderer::Get()->GetQuadMesh()->Draw();
	m_PostFinalShader->UnBind();*/
}

void PostProcessRenderer::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("Post Processing"))
	{
		ImGui::Indent();

		bool enableBloom = m_EnableBloom;
		if(ImGui::Checkbox("Bloom", &enableBloom)) m_EnableBloom = enableBloom;
		
		float filterRad = m_BloomFilterRadius;
		if (ImGui::DragFloat("Filter Radius", &filterRad, 0.001f, 0.001f, 1.0f)) m_BloomFilterRadius = filterRad;

		float bloomStrength = m_BloomStrength;
		if (ImGui::DragFloat("Strength", &bloomStrength, 0.001f, 0.0f, 1.0f)) m_BloomStrength = bloomStrength;

		float brightness = m_BrightnessThreshold;
		if (ImGui::DragFloat("Threshold", &brightness, 0.01f, 0.0f, 1.0f)) m_BrightnessThreshold = brightness;

		ImGui::Unindent();
	}
}