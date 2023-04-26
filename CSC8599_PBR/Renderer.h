#pragma once
#include "..\nclgl\OGLRenderer.h"

class Camera;
class Mesh;
class MeshMaterial;

class Renderer : public OGLRenderer
{
public:
	Renderer(Window& parent);
	~Renderer(void);

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	Camera* mainCamera;
};