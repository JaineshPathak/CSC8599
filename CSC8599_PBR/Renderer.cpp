#include "Renderer.h"
#include "LookAtCamera.h"
#include "ImGuiRenderer.h"
#include "LightsManager.h"
#include <nclgl/Texture.h>
#include <nclgl/TextureHDR.h>
#include <nclgl/TextureEnvCubeMap.h>
#include <nclgl/Light.h>
#include <nclgl/FrameBufferFP.h>
#include <nclgl/FrameBufferHDR.h>
#include <nclgl/UniformBuffer.h>
#include <imgui/imgui_internal.h>

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
	CaptureCubeMap();

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
	//m_PBRShader = std::shared_ptr<Shader>(new Shader("PBR/PBRTexturedVertex.glsl", "PBR/PBRTexturedFragmentBlinnPhong.glsl"));
	if (!m_PBRShader->LoadSuccess()) return false;

	m_PBRBillboardShader = std::shared_ptr<Shader>(new Shader("PBR/PBRBillboardVertex.glsl", "PBR/PBRBillboardFragment.glsl"));
	if (!m_PBRBillboardShader->LoadSuccess()) return false;

	m_CubeMapShader = std::shared_ptr<Shader>(new Shader("PBR/PBRSkyBox2Vertex.glsl", "PBR/PBRSkyBox2Fragment.glsl"));
	if (!m_CubeMapShader->LoadSuccess()) return false;

	m_EquiRect2CubeMapShader = std::shared_ptr<Shader>(new Shader("PBR/PBREquiRect2CubeMapVertex.glsl", "PBR/PBREquiRect2CubeMapFragment.glsl"));
	if (!m_EquiRect2CubeMapShader->LoadSuccess()) return false;

	return true;
}

bool Renderer::InitBuffers()
{
	float w = m_WindowParent.GetScreenSize().x;
	float h = m_WindowParent.GetScreenSize().y;

	m_GlobalFrameBuffer = std::shared_ptr<FrameBufferFP>(new FrameBufferFP(w, h));
	if (m_GlobalFrameBuffer == nullptr) return false;

	m_CaptureFrameBuffer = std::shared_ptr<FrameBufferHDR>(new FrameBufferHDR(512, 512));
	if (m_CaptureFrameBuffer == nullptr) return false;

	m_MatricesUBO = std::shared_ptr<UniformBuffer>(new UniformBuffer(2 * sizeof(Matrix4), NULL, GL_STATIC_DRAW, 0, 0));
	if (!m_MatricesUBO->IsInitialized()) return false;	

	return true;
}

bool Renderer::InitLights()
{
	m_LightsManager = std::shared_ptr<LightsManager>(new LightsManager());
	return m_LightsManager->IsInitialized();
}

bool Renderer::InitMesh()
{
	m_HelmetMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("Mesh_SciFi_Helmet.msh"));
	if(m_HelmetMesh == nullptr) return false;

	m_QuadMesh = std::shared_ptr<Mesh>(Mesh::GenerateQuad());
	if (m_QuadMesh == nullptr) return false;

	m_CubeMesh = std::shared_ptr<Mesh>(Mesh::GenerateCube());
	if (m_CubeMesh == nullptr) return false;

	return true;
}

void Renderer::LoadTexture(const std::string& filePath)
{
	m_HelmetTextureAlbedo = std::shared_ptr<Texture>(new Texture(filePath));
}

bool Renderer::InitTextures()
{
	/*m_HelmetTextureAlbedo = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_BaseColor_sRGB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureAlbedo == 0) return false;

	m_HelmetTextureNormal = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_Normal_Raw.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureNormal == 0) return false;

	m_HelmetTextureMetallic = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_Metallic_Raw.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureMetallic == 0) return false;

	m_HelmetTextureRoughness = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_Roughness_Raw.png", SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureRoughness == 0) return false;

	m_HelmetTextureEmissive = SOIL_load_OGL_texture(TEXTUREDIR"Helmet/Helmet_Emissive_sRGB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_HelmetTextureEmissive == 0) return false;*/

	m_HelmetTextureAlbedo = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_BaseColor_sRGB.png"));
	if (!m_HelmetTextureAlbedo->IsInitialized()) return false;

	m_HelmetTextureNormal = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Normal_Raw.png"));
	if (!m_HelmetTextureNormal->IsInitialized()) return false;

	m_HelmetTextureMetallic = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Metallic_Raw.png"));
	if (!m_HelmetTextureMetallic->IsInitialized()) return false;

	m_HelmetTextureRoughness = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Roughness_Raw.png"));
	if (!m_HelmetTextureRoughness->IsInitialized()) return false;

	m_HelmetTextureEmissive = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Emissive_sRGB.png"));
	if (!m_HelmetTextureEmissive->IsInitialized()) return false;

	m_CubeMapTexture = SOIL_load_OGL_cubemap(
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);
	if (m_CubeMapTexture == 0) return false;

	m_CubeMapHDRTexture = std::shared_ptr<TextureHDR>(new TextureHDR(TEXTUREDIR"HDR/clarens_night_02_2k.hdr"));
	if (!m_CubeMapHDRTexture->IsInitialized()) return false;

	m_CubeMapEnvTexture = std::shared_ptr<TextureEnvCubeMap>(new TextureEnvCubeMap(512, 512));
	if (!m_CubeMapEnvTexture->IsInitialized()) return false;
	
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

	if (ImGuiRenderer::Get()->IsMouseOverScene() && ImGui::GetIO().MouseClicked[0])
	{
		if (ImGui::GetIO().MouseDown[0])
		{
			m_showCursor = true;
			ImGui::GetIO().MouseDrawCursor = m_showCursor;			
			m_WindowParent.LockMouseToWindow(true);
		}
	}
	else if (ImGui::GetIO().MouseReleased[0])
	{
		m_showCursor = true;
		ImGui::GetIO().MouseDrawCursor = m_showCursor;
		m_WindowParent.LockMouseToWindow(false);
	}
}

void Renderer::CaptureCubeMap()
{
	m_EquiRect2CubeMapShader->Bind();
	m_EquiRect2CubeMapShader->SetMat4("proj", m_CaptureFrameBuffer->GetCaptureProjection());
	m_EquiRect2CubeMapShader->SetTexture("equiRectTex", m_CubeMapHDRTexture->GetID(), 0);

	m_CaptureFrameBuffer->Bind();
	for (unsigned int i = 0; i < 6; i++)
	{
		m_EquiRect2CubeMapShader->SetMat4("view", m_CaptureFrameBuffer->GetCaptureViews()[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubeMapEnvTexture->GetID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_CubeMesh->Draw();
	}
	m_CaptureFrameBuffer->Unbind();
	m_EquiRect2CubeMapShader->UnBind();
}

void Renderer::HandleUBOData()
{
	//Start Offset from 0, size = 64
	//Start Offset from 65, size = 64

	m_MatricesUBO->Bind();
	m_MatricesUBO->BindSubData(0, sizeof(Matrix4), m_MainCamera->GetProjectionMatrix().values);
	m_MatricesUBO->BindSubData(sizeof(Matrix4), sizeof(Matrix4), m_MainCamera->GetViewMatrix().values);
	m_MatricesUBO->Unbind();

	// Removed as Lights UBO Data will only be changed when there is any change in the Light Properties. No need for Polling
	// See LightsManager.cpp
	//m_LightsManager->BindLightUBOData();

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

void Renderer::RenderCubeMap2()
{
	glDepthFunc(GL_LEQUAL);

	m_CubeMapShader->Bind();
	m_CubeMapShader->SetTextureCubeMap("cubeTex", m_CubeMapTexture, 0);
	m_CubeMesh->Draw();
	m_CubeMapShader->UnBind();

	glDepthFunc(GL_LESS);
}

void Renderer::RenderHelmet()
{
	m_PBRShader->Bind();

	/*m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo, 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal, 1);
	m_PBRShader->SetTexture("metallicTex", m_HelmetTextureMetallic, 2);
	m_PBRShader->SetTexture("roughnessTex", m_HelmetTextureRoughness, 3);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive, 4);*/

	m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo->GetID(), 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal->GetID(), 1);
	m_PBRShader->SetTexture("metallicTex", m_HelmetTextureMetallic->GetID(), 2);
	m_PBRShader->SetTexture("roughnessTex", m_HelmetTextureRoughness->GetID(), 3);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive->GetID(), 4);

	/*m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo->GetID(), 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal->GetID(), 1);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive->GetID(), 2);
	m_PBRShader->SetTextureCubeMap("cubeTex", m_CubeMapTexture, 3);*/

	m_PBRShader->SetVector3("cameraPos", m_MainCamera->GetPosition());

	m_PBRShader->SetMat4("modelMatrix", modelMatrix);

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
		m_HelmetMesh->DrawSubMesh(i);

	/*Matrix4 floorModelMat = Matrix4::Translation(Vector3::DOWN) * Matrix4::Rotation(-90.0f, Vector3::RIGHT) * Matrix4::Scale(5.0f);
	m_PBRShader->SetMat4("modelMatrix", floorModelMat);
	m_QuadMesh->Draw();*/

	m_PBRShader->UnBind();
}

void Renderer::RenderScene()
{
	m_GlobalFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	HandleUBOData();
	RenderHelmet();
	RenderCubeMap2();

	m_LightsManager->Render();	
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
 