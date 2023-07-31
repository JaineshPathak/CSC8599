#include "Object3DRenderer.h"
#include "Object3DEntity.h"
#include "Renderer.h"
#include "LookAtCamera.h"

#include <nclgl/FrameBuffer.h>
#include <nclgl/ProfilingManager.h>

#include <imgui/imgui.h>

unsigned int Object3DRenderer::m_3DEntityIDs = 0;

Object3DRenderer::Object3DRenderer(const float& width, const float& height) : 
	m_Width(width), m_Height(height),
	m_Current3DEntityIndex(0), m_IsInitialized(false)
{
	m_MainCamera = Renderer::Get()->GetMainCamera();

	if (!InitShaders()) return;
	if (!InitBuffers()) return;
	
	ProfilingManager::RecordTextureTimeStart();
	
	Add3DObject("Car", "Mesh_Car_MiniCooper.msh", "Mesh_Car_MiniCooper.mat", 4.0f);
	//Add3DObject("Helmet", "Mesh_SciFi_Helmet.msh", "Mesh_SciFi_Helmet.mat", 3.0f);
	//Add3DObject("Character", "Mesh_SciFi_Character.msh", "Mesh_SciFi_Character.mat", 1.65f);
	
	ProfilingManager::RecordTextureTimeEnd();

	m_3DEntities[0]->SetPosition(Vector3(0.0f, 0.5f, 0.0f));

	m_3DEntitiesNamesList = new char* [m_3DEntitiesNames.size()];
	for (size_t i = 0; i < m_3DEntitiesNames.size(); i++)
		m_3DEntitiesNamesList[i] = (char*)m_3DEntitiesNames[i].c_str();

	ImGuiRenderer::Get()->RegisterItem(this);

	m_IsInitialized = true;
}

Object3DRenderer::~Object3DRenderer()
{
	delete[] m_3DEntitiesNamesList;
}

bool Object3DRenderer::InitShaders()
{
	m_PBRShader = std::shared_ptr<Shader>(new Shader("PBR/PBRTexturedVertex.glsl", "PBR/PBRTexturedFragment.glsl"));
	m_DepthBufferShader = std::shared_ptr<Shader>(new Shader("PBR/PBRDepthBufferVert.glsl", "PBR/PBRDepthBufferFrag.glsl"));
	if (!m_PBRShader->LoadSuccess() || !m_DepthBufferShader->LoadSuccess())
	{
		std::cout << "ERROR: Object3DRenderer: Failed to load Shader" << std::endl;
		return false;
	}

	return true;
}

bool Object3DRenderer::InitBuffers()
{
	m_DepthFrameBuffer = std::shared_ptr<FrameBuffer>(new FrameBuffer((unsigned int)m_Width, (unsigned int)m_Height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, 1));
	if (m_DepthFrameBuffer == nullptr) return false;

	return true;
}

const unsigned int Object3DRenderer::GetDepthTexture() const
{
	return m_DepthFrameBuffer->GetDepthAttachmentTex();
}

std::shared_ptr<Object3DEntity> Object3DRenderer::Add3DObject(const std::string& objectName, const std::string& objectMeshFile, const std::string& objectMeshMaterialFile, const float& lookAtDistance)
{
	std::shared_ptr<Object3DEntity> entity = std::shared_ptr<Object3DEntity>(new Object3DEntity(objectName, objectMeshFile, objectMeshMaterialFile, "", "", lookAtDistance));
	if (entity != nullptr)
	{
		m_3DEntities[m_3DEntityIDs++] = entity;
		entity->SetObjectShader(m_PBRShader);
		m_3DEntitiesNames.emplace_back(objectName);
	}

	return entity;
}

void Object3DRenderer::Draw()
{
	m_3DEntities[m_Current3DEntityIndex]->Draw();
}

void Object3DRenderer::RenderDepths()
{
	m_DepthFrameBuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	m_DepthBufferShader->Bind();
	
	m_DepthBufferShader->SetMat4("modelMatrix", m_3DEntities[m_Current3DEntityIndex]->GetModelMatrix());
	m_3DEntities[m_Current3DEntityIndex]->Draw();

	m_DepthBufferShader->UnBind();
	m_DepthFrameBuffer->Unbind();
}

void Object3DRenderer::Render()
{
	m_3DEntities[m_Current3DEntityIndex]->Render();
}

void Object3DRenderer::OnImGuiRender()
{
	if (ImGui::CollapsingHeader("3D Objects"))
	{
		ImGui::Indent();

		if (ImGui::Combo("Object Type", &m_Current3DEntityIndex, m_3DEntitiesNamesList, (int)m_3DEntities.size()))
			m_MainCamera->SetLookAtDistance(m_3DEntities[m_Current3DEntityIndex]->GetLookDistance());

		ImGui::Unindent();
	}
}
