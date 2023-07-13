#include "PostProcessSSAO.h"

PostProcessSSAO::PostProcessSSAO(const unsigned int& sizeX, const unsigned int& sizeY) :
    m_SSAOKernelSize(32),
    PostProcessEffect(sizeX, sizeY)
{
    if (!InitShaders()) return;    

    m_LastFBO.~FrameBuffer();
    new(&m_LastFBO) FrameBuffer(m_WidthI, m_HeightI, GL_RGB16F, GL_RGB, GL_FLOAT, 1);

    randomFloats = std::uniform_real_distribution<float>(0.0, 1.0);
    GenerateKernel();
    GenerateNoise();

    ImGuiRenderer::Get()->RegisterPostProcessItem(this);

    m_IsEnabled = false;
}

PostProcessSSAO::~PostProcessSSAO()
{
    m_LastFBO.Destroy();
}

void PostProcessSSAO::GenerateKernel()
{
    m_SSAOKernel.reserve(m_SSAOKernelSize);
    for (int i = 0; i < m_SSAOKernelSize; i++)
    {
        Vector3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample.Normalise();        

        float scale = (float)i / (float)m_SSAOKernelSize;
        scale = naive_lerp(0.1f, 1.0f, scale * scale);
        sample.x *= scale;
        sample.y *= scale;
        sample.z *= scale;

        m_SSAOKernel.emplace_back(sample);
    }
}

void PostProcessSSAO::GenerateNoise()
{    
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

void PostProcessSSAO::Render(const unsigned int& sourceTextureID, const unsigned int& depthTextureID)
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