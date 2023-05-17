#include "LightsManager.h"
#include "Renderer.h"
#include "LookAtCamera.h"
#include <nclgl/Shader.h>
#include <nclgl/UniformBuffer.h>
#include <nclgl/Light.h>

const int MAX_POINT_LIGHTS = 100;

LightsManager::LightsManager()
{
	m_PBRBillboardShader = std::shared_ptr<Shader>(new Shader("PBR/PBRBillboardVertex.glsl", "PBR/PBRBillboardFragment.glsl"));
	if (!m_PBRBillboardShader->LoadSuccess()) { m_IsInitialized = false; return; }

	m_LightIconTexture = SOIL_load_OGL_texture(TEXTUREDIR"Icons/Icon_Light.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (m_LightIconTexture == 0) { m_IsInitialized = false; return; }

	m_LightsUBO = std::shared_ptr<UniformBuffer>(new UniformBuffer(MAX_POINT_LIGHTS * sizeof(PointLight), NULL, GL_DYNAMIC_DRAW, 1, 0));
	if(!m_LightsUBO->IsInitialized()) { m_IsInitialized = false; return; }

	SpawnPointLight();

	ImGuiRenderer::Get()->RegisterItem(this);

	m_IsInitialized = true;
}

LightsManager::~LightsManager()
{
	glDeleteTextures(1, &m_LightIconTexture);
}

void LightsManager::SpawnPointLight()
{
	SpawnPointLight(Vector3(0, 0, 0), Vector4());
}

void LightsManager::SpawnPointLight(const Vector3& lightPosition, const Vector4& lightColor)
{
	std::shared_ptr<Light> newPointLight = std::shared_ptr<Light>(new Light(lightPosition, lightColor, 2.0f));
	m_PointLights.insert(newPointLight);
}

void LightsManager::BindLightUBOData()
{
	m_LightsUBO->Bind();
	int numLights[1] = { (int)m_PointLights.size() };
	m_LightsUBO->BindSubData(0, sizeof(int), numLights);
	for (const auto& light : m_PointLights)
	{
		PointLight pLightStruct;
		pLightStruct.lightPosition = light->GetPosition();
		pLightStruct.lightColor = light->GetColour();
		m_LightsUBO->BindSubData(sizeof(int), sizeof(PointLight), &pLightStruct);
	}
	m_LightsUBO->Unbind();
}

void LightsManager::Render()
{
	if ((int)m_PointLights.size() <= 0) return;

	m_PBRBillboardShader->Bind();
	m_PBRBillboardShader->SetTexture("mainTex", m_LightIconTexture, 0);
	for (const auto& light : m_PointLights)
	{
		Vector3 look = Renderer::Get()->GetMainCamera()->GetPosition() - light->GetPosition();
		look.Normalise();

		//Point Billboards
		Vector3 right = Vector3::Cross(Renderer::Get()->GetMainCamera()->GetUp(), look);
		Vector3 up = Vector3::Cross(look, right);

		Matrix4 billboardMat = Matrix4::Scale(0.3f) * Matrix4::CreateBillboardMatrix(right, up, look, light->GetPosition());
		m_PBRBillboardShader->SetMat4("billboardMatrix", billboardMat, true);

		Renderer::Get()->GetQuadMesh()->Draw();
	}
	m_PBRBillboardShader->UnBind();
}

void LightsManager::OnImGuiRender()
{
	if ((int)m_PointLights.size() <= 0) return;

	if (ImGui::CollapsingHeader("Point Lights"))
	{
		int i = 0;
		for (auto& light : m_PointLights)
		{
			const std::string lightHeaderStr = "Light - [" + std::to_string(i) + "]";
			if (ImGui::CollapsingHeader(lightHeaderStr.c_str()))
			{
				ImGui::Indent();
				Vector3 m_LightPos = light->GetPosition();
				if (ImGui::DragFloat3("Position", (float*)&m_LightPos)) light->SetPosition(m_LightPos);

				Vector4 m_LightColor = light->GetColour();
				if (ImGui::ColorEdit4("Color", (float*)&m_LightColor)) light->SetColour(m_LightColor);

				float m_LightRadius = light->GetRadius();
				if (ImGui::DragFloat("Radius", &m_LightRadius, 1.0f, 0.1f, 1000.0f)) light->SetRadius(m_LightRadius);
				ImGui::Unindent();
			}
			i++;
		}

		ImGui::Separator();
		if (ImGui::Button("New Point Light")) SpawnPointLight();
	}
}