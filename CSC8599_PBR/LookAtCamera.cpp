#include "LookAtCamera.h"

#include <nclgl/Window.h>
#include <imgui/imgui.h>

LookAtCamera::LookAtCamera() :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), Camera()
{
	ImGuiRenderer::Get()->RegisterItem(this);
}

LookAtCamera::LookAtCamera(float _pitch, float _yaw, float _roll, Vector3 _position) :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), Camera(_pitch, _yaw, _roll, _position)
{
	ImGuiRenderer::Get()->RegisterItem(this);
}

LookAtCamera::LookAtCamera(Vector3 _position, Vector3 _rotation) :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), Camera(_position, _rotation)
{
	ImGuiRenderer::Get()->RegisterItem(this);
}

void LookAtCamera::UpdateCamera(float dt)
{
	if (ImGui::GetCurrentContext() == nullptr) return;
	if (!ImGui::GetIO().MouseDrawCursor) return;
	//if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemFocused()) return;

	if (Window::GetMouse()->ButtonHeld(MouseButtons::MOUSE_LEFT))
	{
		m_CamRotation.x -= (Window::GetMouse()->GetRelativePosition().y) * m_Sensitivity;
		m_CamRotation.y -= (Window::GetMouse()->GetRelativePosition().x) * m_Sensitivity;

		m_CamRotation.x = std::min(m_CamRotation.x, 89.0f);
		m_CamRotation.x = std::max(m_CamRotation.x, -89.0f);

		if (m_CamRotation.y < 0)		m_CamRotation.y += 360.0f;
		if (m_CamRotation.y > 360.0f)	m_CamRotation.y -= 360.0f;

		Matrix4 yawMat = Matrix4::Rotation(m_CamRotation.y, Vector3(0, 1, 0));
		Matrix4 pitchMat = Matrix4::Rotation(m_CamRotation.x, yawMat * Vector3(1, 0, 0));
		Matrix4 finalRotMat = pitchMat * yawMat;
	
		Vector3 lookDirection = finalRotMat * Vector3(0, 0, -1);

		m_CamPosition = m_lookAtPos - lookDirection * m_lookAtDistance;
	}
}

Matrix4 LookAtCamera::CalcViewMatrix()
{
	m_CamViewMat = Matrix4::BuildViewMatrix(getPosition(), m_lookAtPos);
	return m_CamViewMat;
}

void LookAtCamera::OnImGuiRender()
{	
	if (ImGui::CollapsingHeader("Camera"))
	{
		float m_sens = m_Sensitivity;
		if (ImGui::SliderFloat("Sensitivity", &m_sens, 1.0f, 8.0f)) m_Sensitivity = m_sens;

		float m_fov = m_FOV;
		if (ImGui::SliderFloat("FOV", &m_fov, 10.0f, 80.0f)) SetFOV(m_fov);
	}
}