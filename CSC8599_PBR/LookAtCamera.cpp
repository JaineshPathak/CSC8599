#include "LookAtCamera.h"

#include <nclgl/Window.h>
#include <imgui/imgui.h>

void LookAtCamera::UpdateCamera(float dt)
{
	if (ImGui::GetCurrentContext() != nullptr && !ImGui::GetIO().MouseDrawCursor)
		return;

	if (Window::GetMouse()->ButtonHeld(MouseButtons::MOUSE_LEFT))
	{
		camRotation.x -= (Window::GetMouse()->GetRelativePosition().y) * 2.0f;
		camRotation.y -= (Window::GetMouse()->GetRelativePosition().x) * 2.0f;

		camRotation.x = std::min(camRotation.x, 89.0f);
		camRotation.x = std::max(camRotation.x, -89.0f);

		if (camRotation.y < 0)
			camRotation.y += 360.0f;

		if (camRotation.y > 360.0f)
			camRotation.y -= 360.0f;

		Matrix4 yawMat = Matrix4::Rotation(camRotation.y, Vector3(0, 1, 0));
		Matrix4 pitchMat = Matrix4::Rotation(-camRotation.x, yawMat * Vector3(-1, 0, 0));
		Matrix4 finalRotMat = pitchMat * yawMat;
	
		Vector3 lookDirection = finalRotMat * Vector3(0, 0, -1);

		camPosition = m_lookAtPos - lookDirection * m_lookAtDistance;
	}
}

Matrix4 LookAtCamera::BuildViewMatrix()
{
	return Matrix4::BuildViewMatrix(getPosition(), m_lookAtPos);
}
