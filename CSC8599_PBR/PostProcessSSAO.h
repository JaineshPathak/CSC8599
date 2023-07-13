#pragma once
#include "PostProcessEffect.h"
#include <random>

class PostProcessSSAO : public PostProcessEffect
{
public:
	PostProcessSSAO(const unsigned int& sizeX, const unsigned int& sizeY);
	~PostProcessSSAO();

private:
	void GenerateKernel();
	void GenerateNoise();

protected:
	virtual bool InitShaders() override;

public:
	virtual const unsigned int GetProcessedTexture() const override;
	virtual void Render(const unsigned int& sourceTextureID, const unsigned int& depthTextureID) override;
	virtual void OnImGuiRender() override;

private:
	std::uniform_real_distribution<float> randomFloats;
	std::default_random_engine generator;

	int m_SSAOKernelSize;
	std::vector<Vector3> m_SSAOKernel;
	std::vector<Vector3> m_SSAONoise;

	std::shared_ptr<Shader> m_PostSSAOShader;
	
	std::shared_ptr<Texture> m_NoiseTexture;
	std::shared_ptr<Texture> m_BlurTexture;
	
	FrameBuffer m_LastFBO;
};