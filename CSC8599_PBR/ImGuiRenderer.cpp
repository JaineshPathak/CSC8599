#include "ImGuiRenderer.h"

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

	ImGui::DockSpaceOverViewport();
	
	ImGui::Begin("Settings");
	for (const auto& elem : m_ImGuiItems)
		elem->OnImGuiRender();	
	ImGui::End();

	ImGui::Begin("Viewport");
	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	ImGui::Image((void*)m_GlobalFrameBuffer->GetColorAttachmentTex(), viewportPanelSize, ImVec2(0, 1), ImVec2(1, 0));
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiRenderer::RegisterItem(IImguiItem* _newItem)
{
	m_ImGuiItems.insert(_newItem);
}