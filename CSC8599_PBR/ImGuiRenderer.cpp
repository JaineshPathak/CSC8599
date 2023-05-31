#include "ImGuiRenderer.h"
#include "Renderer.h"
#include "LookAtCamera.h"

#include <imgui/imgui_internal.h>

ImGuiRenderer* ImGuiRenderer::m_ImGuiRenderer = nullptr;

ImGuiRenderer::ImGuiRenderer(Window& parent)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(parent.GetHandle());
	ImGui_ImplOpenGL3_Init();

	m_ViewportSize.x = parent.GetScreenSize().x;
	m_ViewportSize.y = parent.GetScreenSize().y;

	m_MouseOverScene = false;

	m_ImGuiRenderer = this;
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool ImGuiRenderer::IsInitialised()
{
	return ImGui::GetCurrentContext() != nullptr;
}

void ImGuiRenderer::Render()
{
	if ((int)m_ImGuiItems.size() <= 0) return;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	//if (ImGui::GetCurrentContext()->CurrentWindow != nullptr)
		//std::cout << "Current Window Name: " << ImGui::GetCurrentContext()->CurrentWindow->ID << std::endl;
	//if(ImGui::GetCurrentContext()->HoveredWindow != nullptr)
		//std::cout << "Hovered Window Name: " << ImGui::GetCurrentContext()->HoveredWindow->Name << std::endl;

	ImGui::DockSpaceOverViewport();
	
	ImGui::Begin("Settings");	
	for (const auto& elem : m_ImGuiItems) 
		elem->OnImGuiRender();	
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Scene");

	m_MouseOverScene = ImGui::IsWindowHovered();
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	if (m_ViewportSize.x != viewportPanelSize.x || m_ViewportSize.y != viewportPanelSize.y)
	{
		m_ViewportSize = viewportPanelSize;
		Renderer::Get()->GetGlobalFrameBuffer()->Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);
		Renderer::Get()->GetMainCamera()->SetAspectRatio(m_ViewportSize.x, m_ViewportSize.y);
	}

	ImGui::Image((void*)Renderer::Get()->GetGlobalFrameBuffer()->GetColorAttachmentTex(), viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRenderer::RegisterItem(IImguiItem* _newItem)
{
	m_ImGuiItems.insert(_newItem);
}