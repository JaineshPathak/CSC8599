#include "PostProcessRenderer.h"
#include "Renderer.h"

#include <nclgl/Shader.h>
#include <nclgl/Texture.h>
#include <nclgl/Mesh.h>

PostProcessRenderer::PostProcessRenderer(const unsigned int& sizeX, const unsigned int& sizeY) :
	m_WidthF((float)sizeX), m_HeightF((float)sizeY),
	m_WidthI(sizeX), m_HeightI(sizeY),
	m_BloomIterations(5),
	m_EnableBloom(true),
	m_EnableBloomDebug(false),
	m_BloomFilterRadius(0.005f),
	m_BrightnessThreshold(1.0f),
	m_BrightnessSoftThreshold(0.5f),
	m_BloomStrength(1.0f),
	m_BloomTint(Vector4::WHITE),
	m_BloomTintStrength(1.0f),
	m_EnableDirtMask(false),
	m_DirtMaskStrength(1.0f)
{
	if (!InitShaders()) { m_IsInitialized = false; return; }

	m_BloomFBO.~FrameBufferBloom();
	new(&m_BloomFBO) FrameBufferBloom(m_WidthI, m_HeightI, m_BloomIterations);

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
	m_DirtMaskTexture = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"PostProcess/LensDirt02.png"));
	if (!m_DirtMaskTexture->IsInitialized()) return false;

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

//Prefilter Stage, where only bright colors are filtered based on threshold value are sent for downsampling and later upsampling
void PostProcessRenderer::RenderBrightColors(unsigned int srcTexture)
{
	m_BrightenFBO.Bind();
	m_PostBloomBrightenShader->Bind();
	m_PostBloomBrightenShader->SetTexture("srcTexture", srcTexture, 0);
	m_PostBloomBrightenShader->SetFloat("brightnessThreshold", m_BrightnessThreshold);
	m_PostBloomBrightenShader->SetFloat("brightnessSoftThreshold", m_BrightnessSoftThreshold);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Renderer::Get()->GetQuadMesh()->Draw();

	m_PostBloomBrightenShader->UnBind();
	m_BrightenFBO.Unbind();
}

//Downsampling stage
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

//Upsampling Stage
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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void PostProcessRenderer::OnIterationsChanged()
{
	m_BloomFBO.Recalculate(m_BloomIterations);
}

unsigned int PostProcessRenderer::GetBloomTexture(int index)
{
	return m_BloomFBO.MipChain()[index].texture;
}

unsigned int PostProcessRenderer::GetFinalTexture()
{
	return m_EnableBloomDebug ? m_BrightenFBO.GetColorAttachmentTex() : m_FinalFBO.GetColorAttachmentTex();
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
	m_PostFinalShader->SetTexture("dirtMaskTexture", m_DirtMaskTexture->GetID(), 2);

	m_PostFinalShader->SetFloat("bloomStrength", m_BloomStrength);
	m_PostFinalShader->SetFloat("bloomTintStrength", m_BloomTintStrength);
	m_PostFinalShader->SetVector4("bloomTint", m_BloomTint);

	m_PostFinalShader->SetInt("enableDirtMask", m_EnableDirtMask);
	m_PostFinalShader->SetFloat("dirtMaskStrength", m_DirtMaskStrength);
	
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

		bool enableDebug = m_EnableBloomDebug;
		if (ImGui::Checkbox("Debug", &enableDebug)) m_EnableBloomDebug = enableDebug;

		int iterations = m_BloomIterations;
		if (ImGui::SliderInt("Iterations", &iterations, 1, 8))
		{
			m_BloomIterations = iterations;
			OnIterationsChanged();
		}

		ImGui::Spacing();
		
		float filterRad = m_BloomFilterRadius;
		if (ImGui::DragFloat("Filter Radius", &filterRad, 0.001f, 0.001f, 1.0f)) m_BloomFilterRadius = filterRad;

		float bloomStrength = m_BloomStrength;
		if (ImGui::DragFloat("Strength", &bloomStrength, 0.01f, 0.0f, 100.0f)) m_BloomStrength = bloomStrength;

		ImGui::Spacing();

		float brightnessThres = m_BrightnessThreshold;
		if (ImGui::DragFloat("Threshold", &brightnessThres, 0.01f, 0.0f, 10.0f)) m_BrightnessThreshold = brightnessThres;

		float brightnessSoftThres = m_BrightnessSoftThreshold;
		if (ImGui::DragFloat("Soft Threshold", &brightnessSoftThres, 0.01f, 0.0f, 1.0f)) m_BrightnessSoftThreshold = brightnessSoftThres;

		ImGui::Spacing();

		Vector4 tint = m_BloomTint;
		if (ImGui::ColorEdit4("Tint", (float*)&tint)) m_BloomTint = tint;

		float bloomTintStrength = m_BloomTintStrength;
		if (ImGui::DragFloat("Tint Strength", &bloomTintStrength, 0.01f, 0.0f, 100.0f)) m_BloomTintStrength = bloomTintStrength;

		ImGui::Spacing();

		bool enableDirt = m_EnableDirtMask;
		if (ImGui::Checkbox("Dirt Mask", &enableDirt)) m_EnableDirtMask = enableDirt;

		float dirtStrength = m_DirtMaskStrength;
		if (ImGui::DragFloat("Dirt Strength", &dirtStrength, 0.01f, 0.0f, 100.0f)) m_DirtMaskStrength = dirtStrength;

		ImGui::Unindent();
	}
}