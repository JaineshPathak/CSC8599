#pragma once
#include "..\nclgl\Camera.h"

class LookAtCamera : public Camera
{
public:
	LookAtCamera() : m_lookAtDistance(0.0f), Camera() {};
	LookAtCamera(float _pitch, float _yaw, float _roll, Vector3 _position) : m_lookAtDistance(0.0f), Camera(_pitch, _yaw, _roll, _position) {}
	LookAtCamera(Vector3 _position, Vector3 _rotation) : m_lookAtDistance(0.0f), Camera(_position, _rotation) {}

	void SetLookAtPosition(const Vector3& _lookPos) { m_lookAtPos = _lookPos; }
	void SetLookAtDistance(const float& _lookDist) { m_lookAtDistance = _lookDist; }

	void UpdateCamera(float dt = 1.0f) override;
	Matrix4 BuildViewMatrix() override;

protected:
	float m_lookAtDistance;	
	Vector3 m_lookAtPos;
};