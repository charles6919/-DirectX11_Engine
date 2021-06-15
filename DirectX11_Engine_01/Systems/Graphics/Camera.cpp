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
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->projectionMatrix;
}

const XMVECTOR& Camera::GetPositionVector() const
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->posVector;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->pos;
}

const XMVECTOR& Camera::GetRotationVector() const
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->rotVector;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	// TODO: ���⿡ return ���� �����մϴ�.
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
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_forward;
}

const XMVECTOR& Camera::GetRightVector()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_right;
}

const XMVECTOR& Camera::GetBackwardVector()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_backward;
}

const XMVECTOR& Camera::GetLeftVector()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_left;
}

const XMVECTOR& Camera::GetUpVector()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_up;
}

const XMVECTOR& Camera::GetDownVector()
{
	// TODO: ���⿡ return ���� �����մϴ�.
	return this->vec_down;
}

void Camera::UpdateViewMatrix()
{
	//ī�޶��� rotation ��Ʈ���� ���
	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	//ī�޶��� ȸ�� ��Ʈ������ Forward���͸� �̿��Ͽ� ī�޶� �ٶ󺸴� ������ ���� ���͸� ���.
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	// ī�޶��� ������ġ�� camTarget�� ������.
	camTarget += this->posVector;
	//���� ȸ������ ���� Up���͸� ���
	XMVECTOR upDir = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	//View��Ʈ������ �ٽ� ����.
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, upDir);

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(0.0f, this->rot.y, 0.0f);

	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
	this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, vecRotationMatrix);
	this->vec_down = XMVector3TransformCoord(this->DEFAULT_DOWN_VECTOR, vecRotationMatrix);



}
