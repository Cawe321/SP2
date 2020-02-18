#include "Camera2.h"
#include "Application.h"
#include "Mtx44.h"
#include "Utility.h"

Camera2::Camera2()
{
}

Camera2::~Camera2()
{
}

void Camera2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	cameraLock = false;
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera2::Update(double dt)
{
	if (!cameraLock)
	{
		static const float CAMERA_SPEED = 50.f;
		Vector3 view = (target - position).Normalized();
		Vector3 right = view.Cross(up).Normalized();
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		if (Application::IsKeyPressed('A'))
		{
			position = position - (CAMERA_SPEED / 100.f) * right;
			target = position + view;
		}
		if (Application::IsKeyPressed('D'))
		{
			position = position + (CAMERA_SPEED / 100.f) * right;
			target = position + view;
		}
		if (Application::IsKeyPressed('W'))
		{
			position = position + (CAMERA_SPEED / 100.f) * up;
			target = position + view;
		}
		if (Application::IsKeyPressed('S'))
		{
			position = position - (CAMERA_SPEED / 100.f) * up;
			target = position + view;
		}
		if (Application::IsKeyPressed('N'))
		{
			position = position + (CAMERA_SPEED / 100.f) * view;
			target = position + view;
		}
		if (Application::IsKeyPressed('M'))
		{
			position = position - (CAMERA_SPEED / 100.f) * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_LEFT))
		{
			Mtx44 rotation;
			rotation.SetToRotation(dt * CAMERA_SPEED, up.x, up.y, up.z);
			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_RIGHT))
		{
			Mtx44 rotation;
			rotation.SetToRotation(-dt * CAMERA_SPEED, up.x, up.y, up.z);
			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_UP))
		{
			Mtx44 rotation;
			rotation.SetToRotation(dt * CAMERA_SPEED, right.x, right.y, right.z);
			view = rotation * view;
			target = position + view;
		}
		if (Application::IsKeyPressed(VK_DOWN))
		{
			Mtx44 rotation;
			rotation.SetToRotation(-dt * CAMERA_SPEED, right.x, right.y, right.z);
			view = rotation * view;
			target = position + view;
		}

		if (Application::IsKeyPressed('R'))
		{
			Reset();
		}
	}
}
void Camera2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}