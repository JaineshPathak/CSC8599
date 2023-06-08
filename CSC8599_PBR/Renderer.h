#pragma once
#include "..\nclgl\OGLRenderer.h"
#include <vector>
#include <memory>

class LookAtCamera;
class Mesh;
class MeshMaterial;
class ImGuiRenderer;
class LightsManager;
class FrameBufferFP;
class UniformBuffer;
class Light;
class Texture;

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
	bool InitTextures();
	void SetupGLParameters();

	void HandleInputs(float dt);

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

	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_CubeMesh;
	std::shared_ptr<Mesh> m_HelmetMesh;

	unsigned int m_HelmetTextureAlbedo;
	unsigned int m_HelmetTextureNormal;
	unsigned int m_HelmetTextureMetallic;
	unsigned int m_HelmetTextureRoughness;
	unsigned int m_HelmetTextureEmissive;
	unsigned int m_CubeMapTexture;

	/*std::shared_ptr<Texture> m_HelmetTextureAlbedo;
	std::shared_ptr<Texture> m_HelmetTextureNormal;
	std::shared_ptr<Texture> m_HelmetTextureMetallic;
	std::shared_ptr<Texture> m_HelmetTextureRoughness;
	std::shared_ptr<Texture> m_HelmetTextureEmissive;*/

	std::shared_ptr<FrameBufferFP> m_GlobalFrameBuffer;
	std::shared_ptr<UniformBuffer> m_MatricesUBO;
	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;
	std::shared_ptr<LightsManager> m_LightsManager;

private:
	bool m_showCursor;
	Window& m_WindowParent;
};