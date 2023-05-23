#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"
#include <memory>
#include <unordered_set>
#include <vector>

class UniformBuffer;
class Shader;
class Light;
class DirectionalLight;

struct PointLight
{
	Vector4 lightPosition;
	Vector4 lightColor;
	Vector4 lightAttenuationData;	//x = Kc (Constant), y = Kl (Linear), z = Kq (Quadratic), w = 1.0
};

struct DirectionalLightStruct
{
	Vector4 lightDirection;
	Vector4 lightColor;
};

class LightsManager : public IImguiItem
{
public:
	LightsManager();
	~LightsManager();

	const bool IsInitialized() const { return m_IsInitialized; }

	void SpawnPointLight();
	void SpawnPointLight(const Vector3& lightPosition, const Vector4& lightColor);

	void BindPointLightUBOData(int index = -1);
	void BindDirectionalLightUBOData();
	void Render();

	virtual void OnImGuiRender() override;

protected:
	void OnPointLightPropertyChanged(int index, const Vector3& newLightPos, const Vector4& newLightColor);
	void OnDirectionalLightPropertyChanged(const Vector3& newLightDir, const Vector4& newLightColor);

protected:
	bool m_IsInitialized;
	unsigned int m_LightIconTexture;
	
	std::shared_ptr<UniformBuffer> m_PointLightsUBO;
	std::shared_ptr<UniformBuffer> m_DirectionalLightsUBO;
	
	std::shared_ptr<Shader> m_PBRBillboardShader;
	
	std::unordered_set<std::shared_ptr<Light>> m_PointLights;
	std::vector<PointLight> m_PointLightsVec;

	std::shared_ptr<DirectionalLight> m_DirectionalLight;
	DirectionalLightStruct m_DirectionalLightStruct;
};