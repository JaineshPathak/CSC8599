#include "LookAtCamera.h"

#include <nclgl/Window.h>
#include <imgui/imgui.h>

LookAtCamera::LookAtCamera() :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), m_CameraMovementType(0), Camera()
{
	SetFOV(60.0f);
	ImGuiRenderer::Get()->RegisterItem(this);
}

LookAtCamera::LookAtCamera(float _pitch, float _yaw, float _roll, Vector3 _position) :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), m_CameraMovementType(0), Camera(_pitch, _yaw, _roll, _position)
{
	SetFOV(60.0f);
	ImGuiRenderer::Get()->RegisterItem(this);
}

LookAtCamera::LookAtCamera(Vector3 _position, Vector3 _rotation) :
	m_lookAtDistance(0.0f), m_Sensitivity(6.0f), m_CameraMovementType(0), Camera(_position, _rotation)
{
	SetFOV(60.0f);
	ImGuiRenderer::Get()->RegisterItem(this);
}

void LookAtCamera::UpdateCamera(float dt)
{
	if (ImGui::GetCurrentContext() == nullptr) return;
	if (!ImGui::GetIO().MouseDrawCursor) return;
	//if (ImGui::IsWindowHovered() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemFocused()) return;

	if (m_CameraMovementType == 1)
	{
		Camera::UpdateCamera(dt);
		return;
	}

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
	if (m_CameraMovementType == 1)
		return Camera::CalcViewMatrix();

	m_CamViewMat = Matrix4::BuildViewMatrix(getPosition(), m_lookAtPos);
	return m_CamViewMat;
}

void LookAtCamera::OnImGuiRender()
{	
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Combo("Movement Type", &m_CameraMovementType, m_CameraMovementTypeStr, 2);

		ImGui::Separator();

		float m_sens = m_Sensitivity;
		if (ImGui::SliderFloat("Sensitivity", &m_sens, 1.0f, 8.0f)) m_Sensitivity = m_sens;

		float m_fov = m_FOV;
		if (ImGui::SliderFloat("FOV", &m_fov, 10.0f, 80.0f)) SetFOV(m_fov);

		float m_lookDist = m_lookAtDistance;
		if (ImGui::SliderFloat("Look At Distance", &m_lookDist, 2.0f, 10.0f)) SetLookAtDistance(m_lookDist);
	}
}