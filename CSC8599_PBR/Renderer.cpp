#include "Renderer.h"
#include "LookAtCamera.h"
#include "ImGuiRenderer.h"
#include <nclgl/FrameBuffer.h>

#if _DEBUG
#include <iostream>
#define Log(x) std::cout << x << std::endl
#endif

Renderer* Renderer::m_Renderer = nullptr;

Renderer::Renderer(Window& parent) : m_WindowParent(parent), OGLRenderer(parent)
{	
	init = Initialize();
	if (!init) return;

#if _DEBUG
	Log("Main Renderer: Everything is Initialised! Good To Go!");
#endif

	m_Renderer = this;
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
	if (!m_PBRShader->LoadSuccess()) return false;

	m_CubeMapShader = std::shared_ptr<Shader>(new Shader("skyboxVertex.glsl", "skyboxFragment.glsl"));
	if (!m_CubeMapShader->LoadSuccess()) return false;

	return true;
}

bool Renderer::InitMesh()
{
	m_HelmetMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("Mesh_SciFi_Helmet.msh"));
	if(m_HelmetMesh == nullptr) return false;

	m_QuadMesh = std::shared_ptr<Mesh>(Mesh::GenerateQuad());
	if (m_QuadMesh == nullptr) return false;

	return true;
}

bool Renderer::InitTextures()
{
	m_HelmetTextureAlbedo = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_BaseColor_sRGB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureAlbedo == 0) return false;

	m_CubeMap = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (m_CubeMap == 0) return false;
	return true;
}

bool Renderer::InitBuffers()
{
	float w = m_WindowParent.GetScreenSize().x;
	float h = m_WindowParent.GetScreenSize().y;

	m_GlobalFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(w, h));
	return m_GlobalFrameBuffer != nullptr;
}

bool Renderer::InitImGui()
{
	m_ImGuiRenderer = std::shared_ptr<ImGuiRenderer>(new ImGuiRenderer(m_WindowParent));
	return m_ImGuiRenderer->IsInitialised();
}

void Renderer::SetupGLParameters()
{
	m_MainCamera->SetAspectRatio((float)width, (float)height);
	projMatrix = m_MainCamera->GetProjectionMatrix();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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

void Renderer::RenderCubeMap()
{
	glDepthMask(GL_FALSE);

	m_CubeMapShader->Bind();
	m_CubeMapShader->SetMat4("viewMatrix", m_MainCamera->GetViewMatrix());
	m_CubeMapShader->SetMat4("projMatrix", m_MainCamera->GetProjectionMatrix());

	m_QuadMesh->Draw();

	m_CubeMapShader->UnBind();

	glDepthMask(GL_TRUE);
}

void Renderer::RenderHelmet()
{
	m_PBRShader->Bind();

	m_PBRShader->SetTexture("diffuseTex", m_HelmetTextureAlbedo, 0);
	m_PBRShader->SetMat4("modelMatrix", modelMatrix);
	m_PBRShader->SetMat4("viewMatrix", m_MainCamera->GetViewMatrix());
	m_PBRShader->SetMat4("projMatrix", m_MainCamera->GetProjectionMatrix());

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
		m_HelmetMesh->DrawSubMesh(i);

	m_PBRShader->UnBind();
}

void Renderer::RenderScene()
{
	m_GlobalFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	RenderCubeMap();
	RenderHelmet();
		
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
		m_MainCamera->CalcViewMatrix();
	}
}
 