#include "Renderer.h"
#include "LookAtCamera.h"
#include "ImGuiRenderer.h"
#include <nclgl/Light.h>
#include <nclgl/FrameBuffer.h>
#include <nclgl/UniformBuffer.h>

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
	if (!InitCamera())		return false;
	if (!InitShaders())		return false;
	if (!InitBuffers())		return false;
	if (!InitLights())		return false;
	if (!InitMesh())		return false;
	if (!InitTextures())	return false;

	SetupGLParameters();

	return true;
}

bool Renderer::InitImGui()
{
	m_ImGuiRenderer = std::shared_ptr<ImGuiRenderer>(new ImGuiRenderer(m_WindowParent));
	return m_ImGuiRenderer->IsInitialised();
}

bool Renderer::InitCamera()
{
	m_MainCamera = std::shared_ptr<LookAtCamera>(new LookAtCamera(Vector3(0, 1.0f, 3.0f), Vector3(0, 0, 0)));
	m_MainCamera->SetLookAtDistance(m_MainCamera->GetPosition().z - 0.0f);
	m_MainCamera->SetLookAtPosition(Vector3(0, 1.0f, 0));
	return m_MainCamera != nullptr;
}

bool Renderer::InitShaders()
{
	m_PBRShader = std::shared_ptr<Shader>(new Shader("PBR/PBRTexturedVertex.glsl", "PBR/PBRTexturedFragment.glsl"));
	if (!m_PBRShader->LoadSuccess()) return false;

	m_PBRBillboardShader = std::shared_ptr<Shader>(new Shader("PBR/PBRBillboardVertex.glsl", "PBR/PBRBillboardFragment.glsl"));
	if (!m_PBRBillboardShader->LoadSuccess()) return false;

	m_CubeMapShader = std::shared_ptr<Shader>(new Shader("PBR/PBRSkyBoxVertex.glsl", "PBR/PBRSkyBoxFragment.glsl"));
	if (!m_CubeMapShader->LoadSuccess()) return false;

	return true;
}

bool Renderer::InitBuffers()
{
	float w = m_WindowParent.GetScreenSize().x;
	float h = m_WindowParent.GetScreenSize().y;

	m_GlobalFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer(w, h));
	if (m_GlobalFrameBuffer == nullptr) return false;

	m_MatricesUBO = std::shared_ptr<UniformBuffer>(new UniformBuffer(2 * sizeof(Matrix4), NULL, GL_STATIC_DRAW, 0, 0));
	if (!m_MatricesUBO->IsInitialized()) return false;	

	return true;
}

bool Renderer::InitLights()
{
	m_PointLight = std::shared_ptr<Light>(new Light(Vector3(0.0f, 1.0f, 4.0f), Vector4(1.0f, 0.0f, 1.0f, 1.0f), 2.0f));
	return m_PointLight != nullptr;
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

	m_CubeMapTexture = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (m_CubeMapTexture == 0) return false;

	m_LightIconTexture = SOIL_load_OGL_texture(TEXTUREDIR"Icons/Icon_Light.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_LightIconTexture == 0) return false;

	return true;
}

void Renderer::SetupGLParameters()
{
	m_MainCamera->SetAspectRatio((float)width, (float)height);
	projMatrix = m_MainCamera->GetProjectionMatrix();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void Renderer::HandleUBOData()
{
	//Start Offset from 0, size = 64
	//Start Offset from 65, size = 64

	m_MatricesUBO->Bind();
	m_MatricesUBO->BindSubData(0, sizeof(Matrix4), m_MainCamera->GetProjectionMatrix().values);
	m_MatricesUBO->BindSubData(sizeof(Matrix4), sizeof(Matrix4), m_MainCamera->GetViewMatrix().values);
	m_MatricesUBO->Unbind();

	/*glBindBuffer(GL_UNIFORM_BUFFER, m_UBOMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4), m_MainCamera->GetProjectionMatrix().values);			//Start Offset from 0, size = 64
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4), sizeof(Matrix4), m_MainCamera->GetViewMatrix().values);	//Start Offset from 65, size = 64
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
}

void Renderer::RenderCubeMap()
{
	glDepthMask(GL_FALSE);

	m_CubeMapShader->Bind();
	
	m_QuadMesh->Draw();

	m_CubeMapShader->UnBind();

	glDepthMask(GL_TRUE);
}

void Renderer::RenderHelmet()
{
	m_PBRShader->Bind();

	m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo, 0);

	m_PBRShader->SetVector3("cameraPos", m_MainCamera->GetPosition());
	m_PBRShader->SetVector3("lightPos", m_PointLight->GetPosition());
	m_PBRShader->SetVector4("lightColor", m_PointLight->GetColour());
	
	m_PBRShader->SetMat4("modelMatrix", modelMatrix);

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
		m_HelmetMesh->DrawSubMesh(i);

	m_PBRShader->UnBind();
}

void Renderer::RenderBillboards()
{
	m_PBRBillboardShader->Bind();
	m_PBRBillboardShader->SetTexture("mainTex", m_LightIconTexture, 0);
	m_PBRBillboardShader->SetVector4("mainColor", m_PointLight->GetColour());

	Vector3 look = m_MainCamera->GetPosition() - m_PointLight->GetPosition();
	look.Normalise();
	
	//Point Billboards
	Vector3 right = Vector3::Cross(m_MainCamera->GetUp(), look);
	Vector3 up = Vector3::Cross(look, right);

	//Arbitrary Axis Billboards (Up Axis)
	/*Vector3 up = Vector3(0, 1, 0);
	Vector3 right = Vector3::Cross(up, look);
	right.Normalise();
	look = Vector3::Cross(right, up);*/

	//Axis Aligned Billboards (Y Axis)
	/*Vector3 up = Vector3::UP;
	Vector3 right = Vector3::Cross(up, look);*/

	Matrix4 billboardMat = Matrix4::Scale(0.3f) * Matrix4::CreateBillboardMatrix(right, up, look, m_PointLight->GetPosition());
	m_PBRBillboardShader->SetMat4("billboardMatrix", billboardMat, true);

	m_QuadMesh->Draw();

	m_PBRBillboardShader->UnBind();
}

void Renderer::RenderScene()
{
	m_GlobalFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	HandleUBOData();
	RenderCubeMap();
	RenderHelmet();
	RenderBillboards();
		
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
 