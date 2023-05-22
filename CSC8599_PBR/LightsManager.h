#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"
#include <memory>
#include <unordered_set>
#include <vector>

class UniformBuffer;
class Shader;
class Light;

struct PointLight
{
	Vector4 lightPosition;
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

	void BindLightUBOData(int index = -1);
	void Render();

	virtual void OnImGuiRender() override;

protected:
	void OnLightPropertyChanged(int index, const Vector3& newLightPos, const Vector4& newLightColor);

protected:
	bool m_IsInitialized;
	unsigned int m_LightIconTexture;
	std::shared_ptr<UniformBuffer> m_LightsUBO;
	std::shared_ptr<Shader> m_PBRBillboardShader;
	std::unordered_set<std::shared_ptr<Light>> m_PointLights;
	std::vector<PointLight> m_PointLightsVec;
};