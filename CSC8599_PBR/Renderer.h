#pragma once
#include "..\nclgl\OGLRenderer.h"
#include <memory>

class Camera;
class Mesh;
class MeshMaterial;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);	

protected:
	bool Initialize();
	bool InitCamera();
	bool InitMesh();
	bool InitShader();
	void SetupGLParameters();

public:
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	std::shared_ptr<Camera> m_MainCamera;

	std::shared_ptr<Shader> m_PBRShader;

	std::shared_ptr<Mesh> m_HelmetMesh;
	std::shared_ptr<MeshMaterial> m_HelmetMeshMaterial;
};