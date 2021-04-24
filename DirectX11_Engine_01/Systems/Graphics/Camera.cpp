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

void Camera::AdjustRotation(float x, float y, float z)
{
	this->rot = XMFLOAT3(rot.x + x, rot.y + y, rot.z + z);
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
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
}
