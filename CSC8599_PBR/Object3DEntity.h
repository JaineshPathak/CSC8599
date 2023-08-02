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

private:
	std::shared_ptr<SkyboxRenderer> m_SkyboxRenderer;
	std::shared_ptr<PostProcessRenderer> m_PostProcessRenderer;
	std::shared_ptr<LookAtCamera> m_MainCamera;

	int m_ShaderMode;		//0 - PBR, 1 - Blinn Phong
};