#pragma once
#include "Renderer.h"
#include "ImGuiRenderer.h"
#include "IImguiItem.h"

#include <nclgl/Texture.h>
#include <nclgl/Mesh.h>
#include <nclgl/Shader.h>
#include <nclgl/FrameBuffer.h>

#include <memory>

class PostProcessEffect : public IImguiItem
{
public:
	PostProcessEffect(const unsigned int& sizeX, const unsigned int& sizeY);
	virtual ~PostProcessEffect() {}
	
	virtual const unsigned int GetProcessedTexture() const { return 0; }
	virtual void Render(const unsigned int& sourceTextureID, const unsigned int& depthTextureID) {}

	virtual void OnImGuiRender() override {}

	const bool IsEnabled() const { return m_IsEnabled; }
	void SetActive(const bool& status) { m_IsEnabled = status; }

	virtual void OnResize(const unsigned int& newSizeX, const unsigned int& newSizeY) {}

protected:
	virtual bool InitShaders() { return false; }
	virtual bool InitTextures() { return false; }

protected:
	bool m_IsEnabled;
	float m_WidthF, m_HeightF;
	int m_WidthI, m_HeightI;
	Vector2 m_SrcViewportSize;

	std::shared_ptr<Mesh> m_QuadMesh;
};