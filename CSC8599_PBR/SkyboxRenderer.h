#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"

class UniformBuffer;
class Shader;
class Matrix4;
class Texture;
class TextureHDR;
class TextureEnvCubeMap;
class TextureCubeMap;
class FrameBufferFP;
class FrameBufferHDR;
class UniformBuffer;

struct SkyboxData
{
	Vector4 exposure;
};

class SkyboxRenderer : public IImguiItem
{
public:
	SkyboxRenderer();
	~SkyboxRenderer() {}

	const bool IsInitialized() const { return m_IsInitialized; }

	std::shared_ptr<TextureEnvCubeMap> GetIrradianceTexture() { return m_CubeMapIrradianceTexture; }
	std::shared_ptr<TextureEnvCubeMap> GetPreFilterTexture() { return m_CubeMapPreFilterTexture; }
	std::shared_ptr<Texture> GetBRDFLUTTexture() { return m_BRDFLUTTexture; }

protected:
	bool InitShaders();
	bool InitBuffers();
	bool InitTextures();

	void CaptureHDRCubeMap();
	void CaptureIrradianceMap();
	void CapturePreFilterMipMaps();
	void CaptureBRDFLUTMap();

	void RenderSkybox();
	
	void BindSkyboxUBOData();
	void OnSkyboxDataChanged();

public:
	void Render();	
	virtual void OnImGuiRender() override;

protected:
	bool m_IsInitialized;
	bool m_AlreadyCapturedCubeMap;
	bool m_AlreadyCapturedIrradianceMap;
	bool m_AlreadyCapturedPreFilterMipMaps;
	bool m_AlreadyCapturedBRDFLUTMap;

	float m_Exposure;
	float m_Gamma;

	Matrix4 m_CaptureProjection;
	Matrix4 m_CaptureViews[6];

	std::shared_ptr<Shader> m_CubeMapShader;
	std::shared_ptr<Shader> m_EquiRect2CubeMapShader;
	std::shared_ptr<Shader> m_IrradianceCubeMapShader;
	std::shared_ptr<Shader> m_PreFilterCubeMapShader;
	std::shared_ptr<Shader> m_BRDFIntegrateShader;

	std::shared_ptr<TextureCubeMap> m_CubeMapTexture;
	std::shared_ptr<TextureHDR> m_CubeMapHDRTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapEnvTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapIrradianceTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapPreFilterTexture;
	std::shared_ptr<Texture> m_BRDFLUTTexture;

	std::shared_ptr<FrameBufferFP> m_CaptureHDRFrameBuffer;
	std::shared_ptr<FrameBufferFP> m_CaptureIrradianceFrameBuffer;
	std::shared_ptr<FrameBufferHDR> m_CapturePreFilterFrameBuffer;

	SkyboxData m_SkyboxData;
	std::shared_ptr<UniformBuffer> m_SkyboxUBO;
};