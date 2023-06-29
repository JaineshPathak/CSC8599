#pragma once
#include <nclgl/OGLRenderer.h>
#include <vector>
#include <memory>

class LookAtCamera;
class Mesh;
class MeshMaterial;
class ImGuiRenderer;
class LightsManager;
class FrameBufferFP;
class FrameBufferHDR;
class UniformBuffer;
class Texture;
class TextureHDR;
class TextureCubeMap;
class TextureEnvCubeMap;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void) {};

	static Renderer* Get() { return m_Renderer; }

	std::shared_ptr<Mesh> GetQuadMesh() { return m_QuadMesh; }
	std::shared_ptr<LookAtCamera> GetMainCamera() { return m_MainCamera; }
	std::shared_ptr<FrameBufferFP> GetGlobalFrameBuffer() { return m_GlobalFrameBuffer; }

protected:
	bool Initialize();
	bool InitImGui();
	bool InitCamera();
	bool InitShaders();
	bool InitBuffers();
	bool InitLights();
	bool InitMesh();
	void LoadTexture(const std::string& filePath);
	bool InitTextures();
	void SetupGLParameters();

	void HandleInputs(float dt);

	void CaptureHDRCubeMap();
	void CaptureIrradianceMap();
	void CapturePreFilterMipMaps();

	void HandleUBOData();
	void RenderCubeMap();
	void RenderCubeMap2();
	void RenderHelmet();


public:
	void RenderScene() override;
	void RenderImGui();
	void UpdateScene(float dt) override;

protected:
	static Renderer* m_Renderer;

	std::shared_ptr<LookAtCamera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;
	std::shared_ptr<Shader> m_PBRBillboardShader;
	std::shared_ptr<Shader> m_CubeMapShader;
	std::shared_ptr<Shader> m_EquiRect2CubeMapShader;
	std::shared_ptr<Shader> m_IrradianceCubeMapShader;
	std::shared_ptr<Shader> m_PreFilterCubeMapShader;
	std::shared_ptr<Shader> m_CombinedShader;

	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_CubeMesh;
	std::shared_ptr<Mesh> m_HelmetMesh;

	/*unsigned int m_HelmetTextureAlbedo;
	unsigned int m_HelmetTextureNormal;
	unsigned int m_HelmetTextureMetallic;
	unsigned int m_HelmetTextureRoughness;
	unsigned int m_HelmetTextureEmissive;
	unsigned int envCubeMap;
	unsigned int hdrTexture;*/

	Matrix4 m_CaptureProjection;
	Matrix4 m_CaptureViews[6];

	std::shared_ptr<Texture> m_HelmetTextureAlbedo;
	std::shared_ptr<Texture> m_HelmetTextureNormal;
	std::shared_ptr<Texture> m_HelmetTextureMetallic;
	std::shared_ptr<Texture> m_HelmetTextureRoughness;
	std::shared_ptr<Texture> m_HelmetTextureEmissive;

	std::shared_ptr<TextureCubeMap> m_CubeMapTexture;
	std::shared_ptr<TextureHDR> m_CubeMapHDRTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapEnvTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapIrradianceTexture;
	std::shared_ptr<TextureEnvCubeMap> m_CubeMapPreFilterTexture;

	std::shared_ptr<FrameBufferFP> m_GlobalFrameBuffer;
	std::shared_ptr<FrameBufferFP> m_CaptureHDRFrameBuffer;
	std::shared_ptr<FrameBufferFP> m_CaptureIrradianceFrameBuffer;
	std::shared_ptr<FrameBufferHDR> m_CapturePreFilterFrameBuffer;
	//std::shared_ptr<FrameBufferHDR> m_CaptureFrameBuffer;
	std::shared_ptr<UniformBuffer> m_MatricesUBO;

	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;
	std::shared_ptr<LightsManager> m_LightsManager;

private:
	Window& m_WindowParent;

	bool m_showCursor;
	bool m_AlreadyCapturedCubeMap;
	bool m_AlreadyCapturedIrradianceMap;
	bool m_AlreadyCapturedPreFilterMipMaps;
};