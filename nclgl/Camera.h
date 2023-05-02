#pragma once
#include "Matrix4.h"
#include "Vector3.h"

class Camera
{
public:
	Camera(void)
	{
		//yaw = 0.0f;
		//pitch = 0.0;
		m_CamPosition.ToZero();

		m_CamRotation.x = 0.0f;
		m_CamRotation.y = 0.0f;
		m_CamRotation.z = 0.0f;

		m_CamFront = Vector3(0.0f, 0.0f, -1.0f);
		m_CamUp = Vector3(0.0f, 1.0f, 0.0f);

		m_CurrentSpeed = 0.0f;
	}

	Camera(float _pitch, float _yaw, float _roll, Vector3 _position)
	{
		//yaw = _yaw;
		//pitch = _pitch;
		m_CamPosition = _position;

		m_CamRotation.x = _pitch;
		m_CamRotation.y = _yaw;
		m_CamRotation.z = _roll;

		m_CamFront = Vector3(0.0f, 0.0f, -1.0f);
		m_CamUp = Vector3(0.0f, 1.0f, 0.0f);

		m_CurrentSpeed = 0.0f;
	}

	Camera(Vector3 _position, Vector3 _rotation)
	{
		//yaw = _yaw;
		//pitch = _pitch;
		m_CamPosition = _position;
		m_CamRotation = _rotation;

		m_CamFront = Vector3(0.0f, 0.0f, -1.0f);
		m_CamUp = Vector3(0.0f, 1.0f, 0.0f);

		m_CurrentSpeed = 0.0f;
	}

	~Camera(void) {}

	float getDefaultSpeed() const { return m_DefaultSpeed; }
	inline void SetDefaultSpeed(float s) { m_DefaultSpeed = s; }

	virtual void UpdateCamera(float dt = 1.0f);
	virtual Matrix4 BuildViewMatrix();

	Vector3 getPosition() const { return m_CamPosition; }
	void SetPosition(const Vector3& val) { m_CamPosition = val; }

	Vector3 getRotation() const { return m_CamRotation; }
	void SetRotation(const Vector3& val) { m_CamRotation = val; }

	float GetPitch() const { return m_CamRotation.x; }
	void SetPitch(float p) { m_CamRotation.x = p; }

	float GetYaw() const { return m_CamRotation.y; }
	void SetYaw(float val) { m_CamRotation.y = val; }

	float GetRoll() const { return m_CamRotation.z; }
	void SetRoll(float val) { m_CamRotation.z = val; }


protected:
	//float pitch;
	//float yaw;
	float m_CurrentSpeed;
	float m_DefaultSpeed = 30.0f;

	Vector3 m_CamPosition;
	Vector3 m_CamRotation;

	Vector3 m_CamFront;
	Vector3 m_CamUp;
	Vector3 m_CamRight;

	Matrix4 m_CamViewMat;
};