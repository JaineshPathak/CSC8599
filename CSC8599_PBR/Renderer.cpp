#include "Renderer.h"
#include "LookAtCamera.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

Renderer::Renderer(Window& parent) : m_windowParent(parent), OGLRenderer(parent)
{	
	init = Initialize();
	if (!init) return;
}

Renderer::~Renderer(void)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool Renderer::Initialize()
{
	if (!InitCamera())		return false;
	if (!InitShaders())		return false;
	if (!InitMesh())		return false;
	if (!InitTextures())	return false;
	if (!InitImGui())		return false;

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

bool Renderer::InitImGui()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(m_windowParent.GetHandle());
	ImGui_ImplOpenGL3_Init();

	return ImGui::GetCurrentContext() != nullptr;
}

void Renderer::SetupGLParameters()
{
	projMatrix = Matrix4::Perspective(0.01f, 10000.0f, (float)width / (float)height, 45.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Renderer::HandleInputs(float dt)
{
	if (ImGui::GetIO().MouseClicked[1])
	{
		m_showCursor = !m_showCursor;
		ImGui::GetIO().MouseDrawCursor = m_showCursor;
		m_windowParent.LockMouseToWindow(!m_showCursor);
	}
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(m_PBRShader.get());
	BindTexture(m_HelmetTextureAlbedo, 0, "diffuseTex", m_PBRShader.get());

	UpdateShaderMatrices();

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_HelmetTextureAlbedo);
		m_HelmetMesh->DrawSubMesh(i);
	}

	RenderImGui();
}

void Renderer::RenderImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
 