#pragma once
#include "..\nclgl\Camera.h"

class LookAtCamera : public Camera
{
public:
	LookAtCamera() : Camera() {};
	LookAtCamera(float _pitch, float _yaw, float _roll, Vector3 _position) : Camera(_pitch, _yaw, _roll, _position) {}
	LookAtCamera(Vector3 _position, Vector3 _rotation) : Camera(_position, _rotation) {}

	void SetLookAtPosition(const float& _lookPos) {}

	void UpdateCamera(float dt = 1.0f) override;
	Matrix4 BuildViewMatrix() override;

protected:
	Vector3 lookAtPos;
};