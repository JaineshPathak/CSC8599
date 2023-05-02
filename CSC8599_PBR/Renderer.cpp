#include "Renderer.h"
#include "LookAtCamera.h"
#include "ImGuiRenderer.h"
#include <nclgl/FrameBuffer.h>

#if _DEBUG
#include <iostream>
#define Log(x) std::cout << x << std::endl
#endif


Renderer::Renderer(Window& parent) : m_WindowParent(parent), OGLRenderer(parent)
{	
	init = Initialize();
	if (!init) return;

#if _DEBUG
	Log("Main Renderer: Everything is Initialised! Good To Go!");
#endif
}

bool Renderer::Initialize()
{
	if (!InitImGui())		return false;
	if (!InitBuffers())		return false;
	if (!InitCamera())		return false;
	if (!InitShaders())		return false;
	if (!InitMesh())		return false;
	if (!InitTextures())	return false;

	SetupGLParameters();

	return true;
}

bool Renderer::InitCamera()
{
	m_MainCamera = std::shared_ptr<LookAtCamera>(new LookAtCamera(Vector3(0, 1.0f, 4.0f), Vector3(0, 0, 0)));
	m_MainCamera->SetLookAtDistance(m_MainCamera->getPosition().z - 0.0f);
	m_MainCamera->SetLookAtPosition(Vector3(0, 1.0f, 0));
	return m_MainCamera != nullptr;
}

bool Renderer::InitShaders()
{
	m_PBRShader = std::shared_ptr<Shader>(new Shader("TexturedVertex.glsl", "TexturedFragment.glsl"));
	return m_PBRShader->LoadSuccess();
}

bool Renderer::InitMesh()
{
	m_HelmetMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("Mesh_SciFi_Helmet.msh"));
	return m_HelmetMesh != nullptr;
}

bool Renderer::InitTextures()
{
	m_HelmetTextureAlbedo = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_BaseColor_sRGB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	return m_HelmetTextureAlbedo != 0;
}

bool Renderer::InitBuffers()
{
	float w = m_WindowParent.GetScreenSize().x;
	float h = m_WindowParent.GetScreenSize().y;

	m_GlobalFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(w, h));
	if (m_GlobalFrameBuffer != nullptr)
	{
		ImGuiRenderer::Get()->SetFrameBuffer(m_GlobalFrameBuffer);
		return true;
	}

	return false;
}

bool Renderer::InitImGui()
{
	m_ImGuiRenderer = std::shared_ptr<ImGuiRenderer>(new ImGuiRenderer(m_WindowParent));
	return m_ImGuiRenderer->IsInitialised();
}

void Renderer::SetupGLParameters()
{
	projMatrix = Matrix4::Perspective(0.01f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void Renderer::HandleInputs(float dt)
{
	if (ImGui::GetIO().MouseClicked[1])
	{
		m_showCursor = !m_showCursor;
		ImGui::GetIO().MouseDrawCursor = m_showCursor;
		m_WindowParent.LockMouseToWindow(!m_showCursor);
	}
}

void Renderer::RenderScene()
{
	m_GlobalFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(m_PBRShader.get());
	BindTexture(m_HelmetTextureAlbedo, 0, "diffuseTex", m_PBRShader.get());

	UpdateShaderMatrices();

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HelmetTextureAlbedo);
		m_HelmetMesh->DrawSubMesh(i);
	}

	m_GlobalFrameBuffer->Unbind();

	RenderImGui();
}

void Renderer::RenderImGui()
{
	if (m_ImGuiRenderer == nullptr)
		return;

	m_ImGuiRenderer->Render();
}

void Renderer::UpdateScene(float dt)
{
	HandleInputs(dt);

	if (m_MainCamera != nullptr)
	{
		m_MainCamera->UpdateCamera(dt);
		viewMatrix = m_MainCamera->BuildViewMatrix();
	}
}
 