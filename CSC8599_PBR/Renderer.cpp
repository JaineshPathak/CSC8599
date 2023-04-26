#include "Renderer.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{	
	init = Initialize();
	if (!init) return;
}

Renderer::~Renderer(void)
{
}

bool Renderer::Initialize()
{
	if (!InitCamera()) return false;
	if (!InitMesh()) return false;

	SetupGLParameters();

	return true;
}

bool Renderer::InitCamera()
{
	m_MainCamera = std::shared_ptr<Camera>(new Camera(Vector3(0, 0, 0), Vector3(0, 0, 0)));
	return m_MainCamera != nullptr;
}

bool Renderer::InitMesh()
{
	//m_MainCamera = std::shared_ptr<Camera>(new Camera(Vector3(0, 0, 0), Vector3(0, 0, 0)));
	//return m_MainCamera != nullptr;
	return true;
}

bool Renderer::InitShader()
{
	return false;
}

void Renderer::SetupGLParameters()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::UpdateScene(float dt)
{
	if (m_MainCamera)
	{
		m_MainCamera->UpdateCamera(dt);
		viewMatrix = m_MainCamera->BuildViewMatrix();
	}
}
 