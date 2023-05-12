#pragma once
#include "..\nclgl\OGLRenderer.h"
#include <vector>
#include <memory>

class LookAtCamera;
class Mesh;
class MeshMaterial;
class ImGuiRenderer;
class FrameBuffer;
class Light;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void) {};

	static Renderer* Get() { return m_Renderer; }

	std::shared_ptr<LookAtCamera> GetMainCamera() { return m_MainCamera; }
	std::shared_ptr<FrameBuffer> GetGlobalFrameBuffer() { return m_GlobalFrameBuffer; }

protected:
	bool Initialize();
	bool InitImGui();
	bool InitBuffers();
	bool InitCamera();
	bool InitShaders();
	bool InitLights();
	bool InitMesh();
	bool InitTextures();
	void SetupGLParameters();

	void HandleInputs(float dt);

	void RenderCubeMap();
	void RenderHelmet();
	void RenderBillboards();

public:
	void RenderScene() override;
	void RenderImGui();
	void UpdateScene(float dt) override;

protected:
	static Renderer* m_Renderer;
	static std::vector <std::shared_ptr<Light>> m_Lights;

	std::shared_ptr<LookAtCamera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;
	std::shared_ptr<Shader> m_PBRBillboardShader;
	std::shared_ptr<Shader> m_CubeMapShader;

	std::shared_ptr<Mesh> m_QuadMesh;
	std::shared_ptr<Mesh> m_HelmetMesh;
	std::shared_ptr<MeshMaterial> m_HelmetMeshMaterial;

	std::shared_ptr<Light> m_PointLight;

	unsigned int m_HelmetTextureAlbedo;
	unsigned int m_CubeMapTexture;
	unsigned int m_LightIconTexture;

	std::shared_ptr<FrameBuffer> m_GlobalFrameBuffer;
	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

private:
	bool m_showCursor;
	Window& m_WindowParent;
};