#include "Object3DEntity.h"

#include "Renderer.h"
#include "SkyboxRenderer.h"
#include "PostProcessRenderer.h"
#include "LookAtCamera.h"

#include <nclgl/TextureEnvCubeMap.h>

Object3DEntity::Object3DEntity(const std::string& objectName, const std::string& meshFileName, const std::string& meshMaterialName, const std::string& meshShaderVertexFile, const std::string& meshShaderFragmentFile, const float& lookAtDistance) :
	Object3D(objectName, meshFileName, meshMaterialName, meshShaderVertexFile, meshShaderFragmentFile, lookAtDistance)
{
	m_SkyboxRenderer = Renderer::Get()->GetSkyboxRenderer();
	m_PostProcessRenderer = Renderer::Get()->GetPostProcessRenderer();
	m_MainCamera = Renderer::Get()->GetMainCamera();
}

void Object3DEntity::Render()
{
	if(m_SkyboxRenderer == nullptr)
		m_SkyboxRenderer = Renderer::Get()->GetSkyboxRenderer();

	if(m_PostProcessRenderer == nullptr)
		m_PostProcessRenderer = Renderer::Get()->GetPostProcessRenderer();

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

		m_ShaderObject->SetBool("hasOcclusionTex", m_TexOcclusionSet[i] != -1);
		if (m_TexOcclusionSet[i] != -1) m_ShaderObject->SetTexture("occlusionTex", m_TexOcclusionSet[i], 5);

		m_ShaderObject->SetBool("hasEmissiveTex", m_TexEmissionSet[i] != -1);
		if (m_TexEmissionSet[i] != -1) m_ShaderObject->SetTexture("emissiveTex", m_TexEmissionSet[i], 4);

		m_MeshObject->DrawSubMesh(i);
	}

	m_ShaderObject->UnBind();
}
