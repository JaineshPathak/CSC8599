#include "Renderer.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{
	mainCamera = new Camera(-3.0f, 0.0f, 0.0f, Vector3(0, 1.4f, 4.0f));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	init = true;
}

Renderer::~Renderer(void)
{
	delete mainCamera;
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::UpdateScene(float dt)
{
	if (mainCamera)
	{
		mainCamera->UpdateCamera(dt);
		viewMatrix = mainCamera->BuildViewMatrix();
	}
}
