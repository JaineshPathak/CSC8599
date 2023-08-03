#include "Object3DEntity.h"

#include "Renderer.h"
#include "SkyboxRenderer.h"
#include "PostProcessRenderer.h"
#include "LookAtCamera.h"

#include <nclgl/TextureEnvCubeMap.h>

bool Object3DEntity::m_HasRegistered = false;

Object3DEntity::Object3DEntity(const std::string& objectName, const std::string& meshFileName, const std::string& meshMaterialName, const std::string& meshShaderVertexFile, const std::string& meshShaderFragmentFile, const float& lookAtDistance) :
	m_ShaderMode(0),
	m_BaseColor(Vector4::WHITE),
	m_Metallic(1.0f),
	m_Subsurface(0.0f),
	m_Roughness(1.0f),
	m_Sheen(0.0f), m_SheenTint(1.0f),
	m_ClearCoat(0.0f), m_ClearCoatRoughness(1.0f),
	m_Specular(0.5f), m_SpecularTint(1.0f),
	m_Anisotropic(0.0f),
	m_Emission(1.5f),
	Object3D(objectName, meshFileName, meshMaterialName, meshShaderVertexFile, meshShaderFragmentFile, lookAtDistance)
{
	m_SkyboxRenderer = Renderer::Get()->GetSkyboxRenderer();
	m_PostProcessRenderer = Renderer::Get()->GetPostProcessRenderer();
	m_MainCamera = Renderer::Get()->GetMainCamera();

	if (!m_HasRegistered)
	{
		m_HasRegistered = true;
		ImGuiRenderer::Get()->RegisterItem(this);
	}
}

void Object3DEntity::Render()
{
	if(m_SkyboxRenderer == nullptr)
		m_SkyboxRenderer = Renderer::Get()->GetSkyboxRenderer();

	if(m_PostProcessRenderer == nullptr)
		m_PostProcessRenderer = Renderer::Get()->GetPostProcessRenderer();

	switch (m_ShaderMode)
	{
		case 0: RenderPBRMode(); break;
		case 1: RenderBlinnMode(); break;
		case 2: RenderDisneyMode(); break;
	}
}

void Object3DEntity::RenderPBRMode()
{
	m_ShaderObject->Bind();

	m_ShaderObject->SetTextureCubeMap("irradianceTex", m_SkyboxRenderer->GetIrradianceTexture()->GetID(), 6);
	m_ShaderObject->SetTextureCubeMap("prefilterTex", m_SkyboxRenderer->GetPreFilterTexture()->GetID(), 7);
	m_ShaderObject->SetTexture("brdfLUTTex", m_SkyboxRenderer->GetBRDFLUTTexture()->GetID(), 8);

	m_ShaderObject->SetTexture("ssaoTex", m_PostProcessRenderer->GetSSAOProcessedTexture(), 9);
	m_ShaderObject->SetInt("ssaoEnabled", m_PostProcessRenderer->IsSSAOEnabled() && m_PostProcessRenderer->IsEnabled());

	m_ShaderObject->SetVector3("cameraPos", m_MainCamera->GetPosition());
	m_ShaderObject->SetMat4("modelMatrix", m_ModelMatrix);

	for (int i = 0; i < m_MeshObject->GetSubMeshCount(); i++)
	{
		m_ShaderObject->SetBool("hasAlbedoTex", m_TexDiffuseSet[i] != -1);
		if (m_TexDiffuseSet[i] != -1) m_ShaderObject->SetTexture("albedoTex", m_TexDiffuseSet[i], 0);

		m_ShaderObject->SetBool("hasNormalTex", m_TexNormalSet[i] != -1);
		if (m_TexNormalSet[i] != -1) m_ShaderObject->SetTexture("normalTex", m_TexNormalSet[i], 1);

		m_ShaderObject->SetBool("hasMetallicTex", m_TexMetallicSet[i] != -1);
		if (m_TexMetallicSet[i] != -1) m_ShaderObject->SetTexture("metallicTex", m_TexMetallicSet[i], 2);

		m_ShaderObject->SetBool("hasRoughnessTex", m_TexRoughnessSet[i] != -1);
		if (m_TexRoughnessSet[i] != -1) m_ShaderObject->SetTexture("roughnessTex", m_TexRoughnessSet[i], 3);

		m_ShaderObject->SetBool("hasEmissiveTex", m_TexEmissionSet[i] != -1);
		if (m_TexEmissionSet[i] != -1) m_ShaderObject->SetTexture("emissiveTex", m_TexEmissionSet[i], 4);

		m_ShaderObject->SetBool("hasOcclusionTex", m_TexOcclusionSet[i] != -1);
		if (m_TexOcclusionSet[i] != -1) m_ShaderObject->SetTexture("occlusionTex", m_TexOcclusionSet[i], 5);

		m_MeshObject->DrawSubMesh(i);
	}

	m_ShaderObject->UnBind();
}

void Object3DEntity::RenderBlinnMode()
{
	m_ShaderObject->Bind();

	m_ShaderObject->SetTextureCubeMap("cubeTex", m_SkyboxRenderer->GetEnvCubeMapTexture()->GetID(), 4);
	m_ShaderObject->SetTextureCubeMap("irradianceTex", m_SkyboxRenderer->GetIrradianceTexture()->GetID(), 5);

	m_ShaderObject->SetInt("ssaoEnabled", m_PostProcessRenderer->IsSSAOEnabled() && m_PostProcessRenderer->IsEnabled());
	m_ShaderObject->SetTexture("ssaoTex", m_PostProcessRenderer->GetSSAOProcessedTexture(), 6);

	m_ShaderObject->SetVector3("cameraPos", m_MainCamera->GetPosition());
	m_ShaderObject->SetMat4("modelMatrix", m_ModelMatrix);

	for (int i = 0; i < m_MeshObject->GetSubMeshCount(); i++)
	{
		m_ShaderObject->SetBool("hasAlbedoTex", m_TexDiffuseSet[i] != -1);
		if (m_TexDiffuseSet[i] != -1) m_ShaderObject->SetTexture("albedoTex", m_TexDiffuseSet[i], 0);

		m_ShaderObject->SetBool("hasNormalTex", m_TexNormalSet[i] != -1);
		if (m_TexNormalSet[i] != -1) m_ShaderObject->SetTexture("normalTex", m_TexNormalSet[i], 1);

		m_ShaderObject->SetBool("hasEmissiveTex", m_TexEmissionSet[i] != -1);
		if (m_TexEmissionSet[i] != -1) m_ShaderObject->SetTexture("emissiveTex", m_TexEmissionSet[i], 2);

		m_ShaderObject->SetBool("hasOcclusionTex", m_TexOcclusionSet[i] != -1);
		if (m_TexOcclusionSet[i] != -1) m_ShaderObject->SetTexture("occlusionTex", m_TexOcclusionSet[i], 3);

		m_MeshObject->DrawSubMesh(i);
	}

	m_ShaderObject->UnBind();
}

void Object3DEntity::RenderDisneyMode()
{
	m_ShaderObject->Bind();

	m_ShaderObject->SetTextureCubeMap("irradianceTex", m_SkyboxRenderer->GetIrradianceTexture()->GetID(), 6);
	m_ShaderObject->SetTextureCubeMap("prefilterTex", m_SkyboxRenderer->GetPreFilterTexture()->GetID(), 7);
	m_ShaderObject->SetTexture("brdfLUTTex", m_SkyboxRenderer->GetBRDFLUTTexture()->GetID(), 8);

	m_ShaderObject->SetTexture("ssaoTex", m_PostProcessRenderer->GetSSAOProcessedTexture(), 9);
	m_ShaderObject->SetInt("ssaoEnabled", m_PostProcessRenderer->IsSSAOEnabled() && m_PostProcessRenderer->IsEnabled());

	m_ShaderObject->SetVector3("cameraPos", m_MainCamera->GetPosition());
	m_ShaderObject->SetMat4("modelMatrix", m_ModelMatrix);

	for (int i = 0; i < m_MeshObject->GetSubMeshCount(); i++)
	{
		m_ShaderObject->SetBool("hasAlbedoTex", m_TexDiffuseSet[i] != -1);
		if (m_TexDiffuseSet[i] != -1) m_ShaderObject->SetTexture("albedoTex", m_TexDiffuseSet[i], 0);

		m_ShaderObject->SetBool("hasNormalTex", m_TexNormalSet[i] != -1);
		if (m_TexNormalSet[i] != -1) m_ShaderObject->SetTexture("normalTex", m_TexNormalSet[i], 1);

		m_ShaderObject->SetBool("hasMetallicTex", m_TexMetallicSet[i] != -1);
		if (m_TexMetallicSet[i] != -1) m_ShaderObject->SetTexture("metallicTex", m_TexMetallicSet[i], 2);

		m_ShaderObject->SetBool("hasRoughnessTex", m_TexRoughnessSet[i] != -1);
		if (m_TexRoughnessSet[i] != -1) m_ShaderObject->SetTexture("roughnessTex", m_TexRoughnessSet[i], 3);

		m_ShaderObject->SetBool("hasEmissiveTex", m_TexEmissionSet[i] != -1);
		if (m_TexEmissionSet[i] != -1) m_ShaderObject->SetTexture("emissiveTex", m_TexEmissionSet[i], 4);

		m_ShaderObject->SetBool("hasOcclusionTex", m_TexOcclusionSet[i] != -1);
		if (m_TexOcclusionSet[i] != -1) m_ShaderObject->SetTexture("occlusionTex", m_TexOcclusionSet[i], 5);

		m_ShaderObject->SetVector3("u_BaseColor", Vector3(m_BaseColor.x, m_BaseColor.y, m_BaseColor.z));
		m_ShaderObject->SetFloat("u_Metallic", m_Metallic);
		m_ShaderObject->SetFloat("u_Subsurface", m_Subsurface);
		m_ShaderObject->SetFloat("u_Roughness", m_Roughness);
		m_ShaderObject->SetFloat("u_Specular", m_Specular);
		m_ShaderObject->SetFloat("u_SpecularTint", m_SpecularTint);
		m_ShaderObject->SetFloat("u_Anisotropic", m_Anisotropic);
		m_ShaderObject->SetFloat("u_Sheen", m_Sheen);
		m_ShaderObject->SetFloat("u_SheenTint", m_SheenTint);
		m_ShaderObject->SetFloat("u_ClearCoat", m_ClearCoat);
		m_ShaderObject->SetFloat("u_ClearCoatRoughness", m_ClearCoatRoughness);
		m_ShaderObject->SetFloat("u_Emission", m_Emission);

		m_MeshObject->DrawSubMesh(i);
	}

	m_ShaderObject->UnBind();
}

void Object3DEntity::OnImGuiRender()
{
	if (m_ShaderMode == 2)
	{
		ImGui::Begin("Shader Properties");
		switch (m_ShaderMode)
		{
			case 2:
			{
				ImGui::ColorEdit4("Base Color", (float*)&m_BaseColor);

				ImGui::DragFloat("Metallic", &m_Metallic, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Subsurface", &m_Subsurface, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Roughness", &m_Roughness, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Specular", &m_Specular, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Specular Tint", &m_SpecularTint, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Anisotropic", &m_Anisotropic, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Sheen", &m_Sheen, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Sheen Tint", &m_SheenTint, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Clear Coat", &m_ClearCoat, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Clear Coat Roughness", &m_ClearCoatRoughness, 0.01f, 0.0f, 1.0f);

				ImGui::DragFloat("Emission", &m_Emission, 0.01f, 0.0f, 5.0f);
				break;
			}
		}
		ImGui::End();
	}
}