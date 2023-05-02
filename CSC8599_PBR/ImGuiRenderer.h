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

	void SetFrameBuffer(std::shared_ptr<FrameBuffer> _frameBuffer) { m_GlobalFrameBuffer = _frameBuffer; }

protected:
	static ImGuiRenderer* m_ImGuiRenderer;
	std::shared_ptr<FrameBuffer> m_GlobalFrameBuffer;
	std::unordered_set<IImguiItem*> m_ImGuiItems;
};