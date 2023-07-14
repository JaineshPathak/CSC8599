#pragma once
#include <nclgl/OGLRenderer.h>
#include <vector>
#include <memory>

class LookAtCamera;
class Mesh;
class ImGuiRenderer;
class LightsManager;
class SkyboxRenderer;
class PostProcessRenderer;
class FrameBuffer;
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
	std::shared_ptr<Mesh> GetCubeMesh() { return m_CubeMesh; }
	std::shared_ptr<LookAtCamera> GetMainCamera() { return m_MainCamera; }
	std::shared_ptr<FrameBuffer> GetGlobalFrameBuffer() { return m_GlobalFrameBuffer; }
	std::shared_ptr<FrameBuffer> GetDepthFrameBuffer() { return m_DepthFrameBuffer; }
	//std::shared_ptr<FrameBuffer> GetPositionFrameBuffer() { return m_PositionFrameBuffer; }
	//std::shared_ptr<FrameBuffer> GetNormalsFrameBuffer() { return m_NormalsFrameBuffer; }
	std::shared_ptr<PostProcessRenderer> GetPostProcessBuffer() { return m_PostProcessRenderer; }

protected:
	bool Initialize();
	bool InitImGui();
	bool InitCamera();
	bool InitShaders();
	bool InitBuffers();
	bool InitLights();
	bool InitMesh();
	bool InitPostProcessor();
	bool InitTextures();
	void SetupGLParameters();

	void HandleInputs(float dt);	

	void HandleUBOData();
	//void RenderCubeMap();
	//void RenderCubeMap2();
	void RenderHelmet();


public:
	void RenderScene() override;
	void RenderImGui();
	void UpdateScene(float dt) override;

protected:
	static Renderer* m_Renderer;

	std::shared_ptr<LookAtCamera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;
	std::shared_ptr<Shader> m_DepthBufferShader;
	//std::shared_ptr<Shader> m_PositionBufferShader;
	//std::shared_ptr<Shader> m_NormalsBufferShader;
	std::shared_ptr<Shader> m_CombinedShader;

	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_CubeMesh;
	std::shared_ptr<Mesh> m_HelmetMesh;
		
	std::shared_ptr<Texture> m_HelmetTextureAlbedo;
	std::shared_ptr<Texture> m_HelmetTextureNormal;
	std::shared_ptr<Texture> m_HelmetTextureMetallic;
	std::shared_ptr<Texture> m_HelmetTextureRoughness;
	std::shared_ptr<Texture> m_HelmetTextureEmissive;	

	std::shared_ptr<FrameBuffer> m_GlobalFrameBuffer;
	std::shared_ptr<FrameBuffer> m_DepthFrameBuffer;
	//std::shared_ptr<FrameBuffer> m_PositionFrameBuffer;
	//std::shared_ptr<FrameBuffer> m_NormalsFrameBuffer;
	//std::shared_ptr<FrameBufferHDR> m_CaptureFrameBuffer;
	std::shared_ptr<UniformBuffer> m_MatricesUBO;

	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;
	std::shared_ptr<LightsManager> m_LightsManager;
	std::shared_ptr<SkyboxRenderer> m_SkyboxRenderer;
	std::shared_ptr<PostProcessRenderer> m_PostProcessRenderer;

private:
	Window& m_WindowParent;

	bool m_showCursor;	
};