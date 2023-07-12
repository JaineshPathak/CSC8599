#include "PostProcessSSAO.h"

PostProcessSSAO::PostProcessSSAO(const unsigned int& sizeX, const unsigned int& sizeY) :
    PostProcessEffect(sizeX, sizeY)
{
    if (!InitShaders()) return;    

    m_LastFBO.~FrameBuffer();
    new(&m_LastFBO) FrameBuffer(m_WidthI, m_HeightI, GL_RGB16F, GL_RGB, GL_FLOAT, 1);

    ImGuiRenderer::Get()->RegisterPostProcessItem(this);

    m_IsEnabled = false;
}

PostProcessSSAO::~PostProcessSSAO()
{
    m_LastFBO.Destroy();
}

bool PostProcessSSAO::InitShaders()
{
    m_PostSSAOShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostInvertFrag.glsl"));
    if (!m_PostSSAOShader->LoadSuccess()) return false;

    return true;
}

const unsigned int PostProcessSSAO::GetProcessedTexture() const
{
    return 0;
}

void PostProcessSSAO::Render(const unsigned int& sourceTextureID)
{
}

void PostProcessSSAO::OnImGuiRender()
{
    if (ImGui::CollapsingHeader("SSAO"))
    {
        ImGui::Indent();

        bool enable = m_IsEnabled;
        if (ImGui::Checkbox("Enable", &enable)) m_IsEnabled = enable;

        ImGui::Unindent();
    }
}