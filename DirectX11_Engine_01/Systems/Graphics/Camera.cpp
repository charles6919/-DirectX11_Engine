#include "../../stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->posVector = XMLoadFloat3(&this->pos);
	this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::setprojectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->projectionMatrix;
}

const XMVECTOR& Camera::GetPositionVector() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->posVector;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->pos;
}

const XMVECTOR& Camera::GetRotationVector() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->rotVector;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->rot;
}

void Camera::SetPosition(const XMVECTOR& pos)
{
	this->posVector = pos;
	XMStoreFloat3(&this->pos, pos);
	this->UpdateViewMatrix();

}

void Camera::SetPosition(const XMFLOAT3& pos)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->SetPosition(v);
}

void Camera::SetPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(x, y, z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMFLOAT3& pos)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->AdjustPosition(v);
}

void Camera::AdjustPosition(float x, float y, float z)
{
	this->pos = XMFLOAT3(pos.x + x, pos.y + y, pos.z + z);
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const XMVECTOR& rot)
{
	this->rotVector = rot;
	XMStoreFloat3(&this->rot, rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const XMFLOAT3& rot)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->SetRotation(v);
}

void Camera::SetRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(x, y, z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const XMVECTOR& rot)
{
	this->rotVector += rot;
	XMStoreFloat3(&this->rot, this->rotVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(const XMFLOAT3& rot)
{
	const XMVECTOR v = { pos.x, pos.y, pos.z };
	this->AdjustRotation(v);
}

void Camera::AdjustRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(rot.x + x, rot.y + y, rot.z + z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
{ 
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
		return;

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = (sqrt((double)lookAtPos.x * (double)lookAtPos.x + (double)lookAtPos.z * (double)lookAtPos.z));
		pitch = (float)atan(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atan(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(pitch, yaw, 0.0f);
}

const XMVECTOR& Camera::GetForwardVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_forward;
}

const XMVECTOR& Camera::GetRightVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_right;
}

const XMVECTOR& Camera::GetBackwardVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_backward;
}

const XMVECTOR& Camera::GetLeftVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_left;
}

const XMVECTOR& Camera::GetUpVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_up;
}

const XMVECTOR& Camera::GetDownVector()
{
	// TODO: 여기에 return 문을 삽입합니다.
	return this->vec_down;
}

void Camera::UpdateViewMatrix()
{
	//카메라의 rotation 매트릭스 계산
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//카메라의 회전 매트릭스와 Forward벡터를 이용하여 카메라가 바라보는 방향의 전방 벡터를 계산.
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	// 카메라의 현재위치를 camTarget에 더해줌.
	camTarget += this->posVector;
	//현재 회전값에 대한 Up벡터를 계산
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//View매트릭스를 다시 설정.
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);

	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrix);
	this->vec_down = XMVector3TransformCoord(this->DEFAULT_DOWN_VECTOR, vecRotationMatrix);



}
