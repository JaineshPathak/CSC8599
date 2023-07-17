#include "Renderer.h"
#include "LookAtCamera.h"
#include "ImGuiRenderer.h"
#include "LightsManager.h"
#include "SkyboxRenderer.h"
#include "PostProcessRenderer.h"

#include <nclgl/Texture.h>
#include <nclgl/TextureHDR.h>
#include <nclgl/TextureEnvCubeMap.h>
#include <nclgl/TextureCubeMap.h>
#include <nclgl/FrameBuffer.h>
#include <nclgl/FrameBufferFP.h>
#include <nclgl/FrameBufferHDR.h>
#include <nclgl/UniformBuffer.h>
#include <nclgl/MeshMaterial.h>
#include <stb_image/stb_image.h>

#if _DEBUG
#include <iostream>
#define Log(x) std::cout << x << std::endl
#endif

Renderer* Renderer::m_Renderer = nullptr;

Renderer::Renderer(Window& parent) : m_WindowParent(parent), OGLRenderer(parent)
{	
	m_Renderer = this;

	init = Initialize();
	if (!init) return;

#if _DEBUG
	Log("Main Renderer: Everything is Initialised! Good To Go!");
#endif
} 

bool Renderer::Initialize()
{
	if (!InitImGui())				return false;
	if (!InitCamera())				return false;
	if (!InitShaders())				return false;
	if (!InitBuffers())				return false;
	if (!InitLights())				return false;
	if (!InitMesh())				return false;
	if (!InitPostProcessor())		return false;
	if (!InitTextures())			return false;
	if (!InitMaterialTextures())	return false;

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
	m_MainCamera->SetLookAtDistance(m_MainCamera->GetPosition().z - 0.0f + 1.0f);
	m_MainCamera->SetLookAtPosition(Vector3(0, 0.5f, 0));
	return m_MainCamera != nullptr;
}

bool Renderer::InitShaders()
{
	m_PBRShader = std::shared_ptr<Shader>(new Shader("PBR/PBRTexturedVertex.glsl", "PBR/PBRTexturedFragment.glsl"));
	//m_PBRShader = std::shared_ptr<Shader>(new Shader("PBR/PBRTexturedVertex.glsl", "PBR/PBRTexturedFragmentBlinnPhong.glsl"));
	if (!m_PBRShader->LoadSuccess()) return false;

	m_DepthBufferShader = std::shared_ptr<Shader>(new Shader("PBR/PBRDepthBufferVert.glsl", "PBR/PBRDepthBufferFrag.glsl"));
	if (!m_DepthBufferShader->LoadSuccess()) return false;

	/*
	m_PositionBufferShader = std::shared_ptr<Shader>(new Shader("PBR/PBRPositionBufferVert.glsl", "PBR/PBRPositionBufferFrag.glsl"));
	if (!m_PositionBufferShader->LoadSuccess()) return false;

	m_NormalsBufferShader = std::shared_ptr<Shader>(new Shader("PBR/PBRNormalBufferVert.glsl", "PBR/PBRNormalBufferFrag.glsl"));
	if (!m_NormalsBufferShader->LoadSuccess()) return false;
	*/

	m_CombinedShader = std::shared_ptr<Shader>(new Shader("PostProcess/PostBloomVert.glsl", "PostProcess/PostFinalFrag.glsl"));
	if (!m_CombinedShader->LoadSuccess()) return false;		

	return true;
}

bool Renderer::InitBuffers()
{
	float w = m_WindowParent.GetScreenSize().x;
	float h = m_WindowParent.GetScreenSize().y;

	m_GlobalFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer((unsigned int)w, (unsigned int)h, GL_RGBA16F, GL_RGBA, GL_FLOAT, 2));
	if (m_GlobalFrameBuffer == nullptr) return false;

	m_DepthFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer((unsigned int)w, (unsigned int)h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 1));
	if (m_DepthFrameBuffer == nullptr) return false;

	/*
	m_PositionFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer((unsigned int)w, (unsigned int)h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 1));
	if (m_PositionFrameBuffer == nullptr) return false;
	m_PositionFrameBuffer->RemoveDepthAttachment();

	m_NormalsFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer((unsigned int)w, (unsigned int)h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 1));
	if (m_NormalsFrameBuffer == nullptr) return false;
	m_NormalsFrameBuffer->RemoveDepthAttachment();
	*/

	m_MatricesUBO = std::shared_ptr<UniformBuffer>(new UniformBuffer(2 * sizeof(Matrix4), NULL, GL_STATIC_DRAW, 0, 0));
	if (!m_MatricesUBO->IsInitialized()) return false;

	return true;
}

bool Renderer::InitLights()
{
	m_LightsManager = std::shared_ptr<LightsManager>(new LightsManager());
	if(!m_LightsManager->IsInitialized()) return false;

	m_SkyboxRenderer = std::shared_ptr<SkyboxRenderer>(new SkyboxRenderer());
	if (!m_SkyboxRenderer->IsInitialized()) return false;

	return true;
}

bool Renderer::InitMesh()
{
	m_CarMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("Mesh_Car_MiniCooper.msh"));
	if(m_CarMesh == nullptr) return false;

	//m_HelmetMesh = std::shared_ptr<Mesh>(Mesh::LoadFromMeshFile("Mesh_SciFi_Helmet.msh"));
	//if(m_HelmetMesh == nullptr) return false;

	m_QuadMesh = std::shared_ptr<Mesh>(Mesh::GenerateQuad());
	if (m_QuadMesh == nullptr) return false;

	m_CubeMesh = std::shared_ptr<Mesh>(Mesh::GenerateCube());
	if (m_CubeMesh == nullptr) return false;

	return true;
}

bool Renderer::InitTextures()
{
#pragma region Helmet Stuffs
	/*m_HelmetTextureAlbedo = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_BaseColor_sRGB.png"));
if (!m_HelmetTextureAlbedo->IsInitialized()) return false;

m_HelmetTextureNormal = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Normal_Raw.png"));
if (!m_HelmetTextureNormal->IsInitialized()) return false;

m_HelmetTextureMetallic = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Metallic_Raw.png", GL_RED, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, true));
if (!m_HelmetTextureMetallic->IsInitialized()) return false;

m_HelmetTextureRoughness = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Roughness_Raw.png", GL_RED, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, true));
if (!m_HelmetTextureRoughness->IsInitialized()) return false;

m_HelmetTextureEmissive = std::shared_ptr<Texture>(new Texture(TEXTUREDIR"Helmet/Helmet_Emissive_sRGB.png"));
if (!m_HelmetTextureEmissive->IsInitialized()) return false;*/
#pragma endregion

	m_CarMaterial = std::shared_ptr<MeshMaterial>(new MeshMaterial("Mesh_Car_MiniCooper.mat"));
	if (m_CarMaterial == nullptr) return false;
	
	return true;
}

bool Renderer::InitMaterialTextures()
{
	if (m_CarMesh == nullptr)
		return false;

	m_CarDiffuseSet.assign(m_CarMesh->GetSubMeshCount(), -1);
	m_CarMetallicSet.assign(m_CarMesh->GetSubMeshCount(), -1);
	m_CarRoughnessSet.assign(m_CarMesh->GetSubMeshCount(), -1);
	m_CarNormalSet.assign(m_CarMesh->GetSubMeshCount(), -1);
	m_CarOcclusionSet.assign(m_CarMesh->GetSubMeshCount(), -1);
	m_CarEmissionSet.assign(m_CarMesh->GetSubMeshCount(), -1);

	for (int i = 0; i < m_CarMesh->GetSubMeshCount(); i++)
	{
		const MeshMaterialEntry* matEntry = m_CarMaterial->GetMaterialForLayer(i);
		LoadMaterialTextures(i, "Diffuse", matEntry, m_CarDiffuseSet);
		LoadMaterialTextures(i, "Metallic", matEntry, m_CarMetallicSet);
		LoadMaterialTextures(i, "Roughness", matEntry, m_CarRoughnessSet);
		LoadMaterialTextures(i, "Bump", matEntry, m_CarNormalSet);
		LoadMaterialTextures(i, "Occlusion", matEntry, m_CarOcclusionSet);
		LoadMaterialTextures(i, "Emission", matEntry, m_CarEmissionSet);
	}

	return true;
}

bool Renderer::InitPostProcessor()
{
	m_PostProcessRenderer = std::shared_ptr<PostProcessRenderer>(new PostProcessRenderer(width, height));
	if (!m_PostProcessRenderer->IsInitialized()) return false;

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

std::shared_ptr<Texture> Renderer::AddMaterialTexture(const std::string& fileName)
{
	std::unordered_map<std::string, std::shared_ptr<Texture>>::iterator i = m_CarTexturesSet.find(fileName);
	if (i != m_CarTexturesSet.end())
		return i->second;

	std::shared_ptr<Texture> tex = std::shared_ptr<Texture>(new Texture(fileName));
	m_CarTexturesSet.emplace(fileName, tex);

	return tex;
}

void Renderer::LoadMaterialTextures(const int& index, const std::string& entryName, const MeshMaterialEntry* materialEntry, std::vector<int>& textureSetContainer)
{
	const std::string* textureFileName = nullptr;
	materialEntry->GetEntry(entryName, &textureFileName);
	if (textureFileName != nullptr)
	{
		std::string filePath = TEXTUREDIR + *textureFileName;
		std::shared_ptr<Texture> tex = AddMaterialTexture(filePath);
		
		if (tex != nullptr)
			textureSetContainer[index] = tex->GetID();		
	}
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
		ImGui::GetIO().MouseDrawCursor = m_showCursor;
		m_WindowParent.LockMouseToWindow(true);		
	}
	else if (ImGui::GetIO().MouseReleased[0])
	{
		m_showCursor = true;
		ImGui::GetIO().MouseDrawCursor = m_showCursor;
		m_WindowParent.LockMouseToWindow(false);
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

	// Removed as Lights UBO Data will only be changed when there is any change in the Light Properties. No need for Polling
	// See LightsManager.cpp
	//m_LightsManager->BindLightUBOData();
}

void Renderer::RenderHelmet()
{
	m_PBRShader->Bind();

	/*m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo, 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal, 1);
	m_PBRShader->SetTexture("metallicTex", m_HelmetTextureMetallic, 2);
	m_PBRShader->SetTexture("roughnessTex", m_HelmetTextureRoughness, 3);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive, 4);*/

	/*m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo->GetID(), 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal->GetID(), 1);
	m_PBRShader->SetTexture("metallicTex", m_HelmetTextureMetallic->GetID(), 2);
	m_PBRShader->SetTexture("roughnessTex", m_HelmetTextureRoughness->GetID(), 3);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive->GetID(), 4);*/
	
	m_PBRShader->SetTextureCubeMap("irradianceTex", m_SkyboxRenderer->GetIrradianceTexture()->GetID(), 6);
	m_PBRShader->SetTextureCubeMap("prefilterTex", m_SkyboxRenderer->GetPreFilterTexture()->GetID(), 7);
	m_PBRShader->SetTexture("brdfLUTTex", m_SkyboxRenderer->GetBRDFLUTTexture()->GetID(), 8);
	
	m_PBRShader->SetTexture("ssaoTex", m_PostProcessRenderer->GetSSAOProcessedTexture(), 9);
	m_PBRShader->SetInt("ssaoEnabled", m_PostProcessRenderer->IsSSAOEnabled() && m_PostProcessRenderer->IsEnabled());

	/*m_PBRShader->SetTexture("albedoTex", m_HelmetTextureAlbedo->GetID(), 0);
	m_PBRShader->SetTexture("normalTex", m_HelmetTextureNormal->GetID(), 1);
	m_PBRShader->SetTexture("emissiveTex", m_HelmetTextureEmissive->GetID(), 2);
	m_PBRShader->SetTextureCubeMap("cubeTex", m_CubeMapTexture, 3);*/

	m_PBRShader->SetVector3("cameraPos", m_MainCamera->GetPosition());
	m_PBRShader->SetMat4("modelMatrix", modelMatrix);

	for (int i = 0; i < m_CarMesh->GetSubMeshCount(); i++)
	{
		m_PBRShader->SetBool("hasAlbedoTex", m_CarDiffuseSet[i] != -1);
		if(m_CarDiffuseSet[i] != -1) m_PBRShader->SetTexture("albedoTex", m_CarDiffuseSet[i], 0);

		m_PBRShader->SetBool("hasNormalTex", m_CarNormalSet[i] != -1);
		if (m_CarNormalSet[i] != -1) m_PBRShader->SetTexture("normalTex", m_CarNormalSet[i], 1);

		m_PBRShader->SetBool("hasMetallicTex", m_CarMetallicSet[i] != -1);
		if (m_CarMetallicSet[i] != -1) m_PBRShader->SetTexture("metallicTex", m_CarMetallicSet[i], 2);

		m_PBRShader->SetBool("hasRoughnessTex", m_CarRoughnessSet[i] != -1);
		if (m_CarRoughnessSet[i] != -1) m_PBRShader->SetTexture("roughnessTex", m_CarRoughnessSet[i], 3);

		m_PBRShader->SetBool("hasOcclusionTex", m_CarOcclusionSet[i] != -1);
		if (m_CarOcclusionSet[i] != -1) m_PBRShader->SetTexture("occlusionTex", m_CarOcclusionSet[i], 5);

		m_PBRShader->SetBool("hasEmissiveTex", m_CarEmissionSet[i] != -1);
		if (m_CarEmissionSet[i] != -1) m_PBRShader->SetTexture("emissiveTex", m_CarEmissionSet[i], 4);


		m_CarMesh->DrawSubMesh(i);
	}

	/*Matrix4 floorModelMat = Matrix4::Translation(Vector3::DOWN) * Matrix4::Rotation(-90.0f, Vector3::RIGHT) * Matrix4::Scale(5.0f);
	m_PBRShader->SetMat4("modelMatrix", floorModelMat);
	m_QuadMesh->Draw();*/

	m_PBRShader->UnBind();
}

void Renderer::RenderScene()
{	
	//Render the Depths
	m_DepthFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_DepthBufferShader->Bind();

	for (int i = 0; i < m_CarMesh->GetSubMeshCount(); i++)
		m_CarMesh->DrawSubMesh(i);

	m_DepthBufferShader->UnBind();
	m_DepthFrameBuffer->Unbind();

	//----------------------------------------------------------------------------

	//SSAO Pass
	if (m_PostProcessRenderer != nullptr && m_PostProcessRenderer->IsEnabled() && m_PostProcessRenderer->IsSSAOEnabled())
		m_PostProcessRenderer->RenderSSAOPass(m_DepthFrameBuffer->GetDepthAttachmentTex());

	//----------------------------------------------------------------------------

	/*
	//Render the Positions
	m_PositionFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_PositionBufferShader->Bind();
	m_PositionBufferShader->SetMat4("modelMatrix", modelMatrix);

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
		m_HelmetMesh->DrawSubMesh(i);
	
	m_PositionBufferShader->UnBind();
	m_PositionFrameBuffer->Unbind();

	//----------------------------------------------------------------------------

	//Render the Normals
	m_NormalsFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_NormalsBufferShader->Bind();
	m_NormalsBufferShader->SetTexture("normalTex", m_HelmetTextureNormal->GetID(), 0);
	m_NormalsBufferShader->SetMat4("modelMatrix", modelMatrix);

	for (int i = 0; i < m_HelmetMesh->GetSubMeshCount(); i++)
		m_HelmetMesh->DrawSubMesh(i);

	m_NormalsBufferShader->UnBind();
	m_NormalsFrameBuffer->Unbind();

	//----------------------------------------------------------------------------	
	*/
	m_GlobalFrameBuffer->Bind();	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	HandleUBOData();
	RenderHelmet();
	m_SkyboxRenderer->Render();
	m_LightsManager->Render();

	m_GlobalFrameBuffer->Unbind();

	m_PostProcessRenderer->Render(m_GlobalFrameBuffer->GetColorAttachmentTex(1), m_DepthFrameBuffer->GetDepthAttachmentTex());
	RenderImGui();
	
	/*
	//----------------------------------------------------------------------------
	// TESTING AREA
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_CombinedShader->Bind();	
	m_CombinedShader->SetTexture("diffuseTex", m_GlobalFrameBuffer->GetColorAttachmentTex(), 0);
	m_QuadMesh->Draw();
	m_CombinedShader->UnBind();
	//----------------------------------------------------------------------------
	*/
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