#pragma once
#include "IImguiItem.h"
#include "ImGuiRenderer.h"

#include <memory>
#include <unordered_map>

class Object3DEntity;
class Shader;
class LookAtCamera;
class Texture;
class FrameBuffer;
class Object3DRenderer : public IImguiItem
{
public:
	Object3DRenderer(const float& width, const float& height);
	~Object3DRenderer();

protected:
	bool InitShaders();
	bool InitBuffers();

public:
	const bool IsInitialized() const { return m_IsInitialized; }
	const unsigned int GetDepthTexture() const;

private:
	std::shared_ptr<Object3DEntity> Add3DObject(const std::string& objectName, const std::string& objectMeshFile, const std::string& objectMeshMaterialFile, const float& lookAtDistance = 3.0f);
	void ChangeShaderMode(const int& newShaderMode);
	void OnObject3DChanged();

public:
	void Draw();
	void RenderDepths();
	void Render();
	virtual void OnImGuiRender() override;

private:
	float m_Width, m_Height;
	int m_ShaderMode;

	std::shared_ptr<LookAtCamera> m_MainCamera;
	std::shared_ptr<Shader> m_PBRShader;
	std::shared_ptr<Shader> m_BlinnShader;
	std::shared_ptr<Shader> m_DisneyShader;
	std::shared_ptr<Shader> m_OrenNayarShader;
	std::shared_ptr<Shader> m_DepthBufferShader;

	std::shared_ptr<FrameBuffer> m_DepthFrameBuffer;

	static unsigned int s_3DEntityIDs;
	char** m_3DEntitiesNamesList;
	std::vector<std::string> m_3DEntitiesNames;
	int m_Current3DEntityIndex;
	std::unordered_map<unsigned int, std::shared_ptr<Object3DEntity>> m_3DEntities;

	bool m_IsInitialized;
};