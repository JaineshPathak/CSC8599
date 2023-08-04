#pragma once
#include <nclgl/Object3D.h>

class SkyboxRenderer;
class PostProcessRenderer;
class LookAtCamera;
class Object3DEntity : public Object3D
{
public:
	Object3DEntity(const std::string& objectName, const std::string& meshFileName, const std::string& meshMaterialName, const std::string& meshShaderVertexFile = "", const std::string& meshShaderFragmentFile = "", const float& lookAtDistance = 3.0f);
	~Object3DEntity() {}

	const int GetShaderMode() const { return m_ShaderMode; }
	void SetShaderMode(const int& shaderMode) { m_ShaderMode = shaderMode; }

	virtual void Render() override;

private:
	void RenderPBRMode();
	void RenderBlinnMode();
	void RenderDisneyMode();

public:
	virtual void RenderShaderProperties();

private:
	std::shared_ptr<SkyboxRenderer> m_SkyboxRenderer;
	std::shared_ptr<PostProcessRenderer> m_PostProcessRenderer;
	std::shared_ptr<LookAtCamera> m_MainCamera;

	int m_ShaderMode;		//0 - PBR, 1 - Blinn Phong, 2 - Disney	

	Vector4 m_BaseColor;
	float m_Metallic;
	float m_Subsurface;
	float m_Roughness;
	float m_Sheen, m_SheenTint;
	float m_ClearCoat, m_ClearCoatRoughness;
	float m_Specular, m_SpecularTint;
	float m_Anisotropic;
	float m_Emission;
};