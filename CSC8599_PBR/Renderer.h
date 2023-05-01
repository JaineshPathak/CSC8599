#pragma once
#include "..\nclgl\OGLRenderer.h"
#include <memory>

class LookAtCamera;
class Mesh;
class MeshMaterial;
class ImGuiRenderer;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void) {};

protected:
	bool Initialize();
	bool InitCamera();
	bool InitShaders();
	bool InitMesh();
	bool InitTextures();
	bool InitImGui();
	void SetupGLParameters();

	void HandleInputs(float dt);

public:
	void RenderScene() override;
	void RenderImGui();
	void UpdateScene(float dt) override;

protected:
	std::shared_ptr<LookAtCamera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;

	std::shared_ptr<Mesh> m_HelmetMesh;
	unsigned int m_HelmetTextureAlbedo;
	std::shared_ptr<MeshMaterial> m_HelmetMeshMaterial;

	std::shared_ptr<ImGuiRenderer> m_ImGuiRenderer;

private:
	bool m_showCursor;
	Window& m_windowParent;
};