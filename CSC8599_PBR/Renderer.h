#pragma once
#include "..\nclgl\OGLRenderer.h"
#include <memory>

class LookAtCamera;
class Mesh;
class MeshMaterial;
class ImGuiRenderer;
class FrameBuffer;

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
	bool InitCamera();
	bool InitShaders();
	bool InitMesh();
	bool InitTextures();
	bool InitBuffers();
	bool InitImGui();
	void SetupGLParameters();

	void HandleInputs(float dt);

public:
	void RenderScene() override;
	void RenderImGui();
	void UpdateScene(float dt) override;

protected:
	static Renderer* m_Renderer;

	std::shared_ptr<LookAtCamera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;

	std::shared_ptr<Mesh> m_HelmetMesh;
	unsigned int m_HelmetTextureAlbedo;
	std::shared_ptr<MeshMaterial> m_HelmetMeshMaterial;

	std::shared_ptr<FrameBuffer> m_GlobalFrameBuffer;
	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

private:
	bool m_showCursor;
	Window& m_WindowParent;
};