#pragma once

#include "IImguiItem.h"

#include <nclgl/Window.h>
#include <nclgl/FrameBuffer.h>

#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>

#include <unordered_set>
#include <memory>

class ImGuiRenderer
{
public:
	static ImGuiRenderer* Get() { return m_ImGuiRenderer; }
	
	ImGuiRenderer(Window& parent);
	~ImGuiRenderer();

	bool IsInitialised();
	void Render();

	void RegisterItem(IImguiItem* _newItem);
	void RegisterPostProcessItem(IImguiItem* _newItem);

	const bool IsMouseOverScene() const { return m_MouseOverScene; }

protected:
	static ImGuiRenderer* m_ImGuiRenderer;
	std::unordered_set<IImguiItem*> m_ImGuiItems;
	std::unordered_set<IImguiItem*> m_PostProcessImGuiItems;

	ImVec2 m_ViewportSize;
	bool m_MouseOverScene;
};