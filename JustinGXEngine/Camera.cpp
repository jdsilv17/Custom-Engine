#include "Camera.h"

Camera::Camera()
{
	this->Pos_F4 = DirectX::XMFLOAT4(0.0f, 5.0f, -15.0f, 1.0f);
	this->Rot_F3 = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->UpdateViewMatrix();
}

const DirectX::XMMATRIX& Camera::GetViewMatrix() const
{
	return this->View_M;
}

const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->Projection_M;
}

const DirectX::XMVECTOR& Camera::GetPositionVector() const
{
	return this->Pos_V;
}

const DirectX::XMFLOAT4& Camera::GetPositionFloat4() const
{
	return this->Pos_F4;
}

const DirectX::XMVECTOR& Camera::GetRotationVector() const
{
	return this->Rot_V;
}

const DirectX::XMFLOAT3& Camera::GetRotationFloat4() const
{
	return this->Rot_F3;
}

const DirectX::XMVECTOR& Camera::GetForwardVector() const
{
	return this->FORWARD;
}

const DirectX::XMVECTOR& Camera::GetBackwardVector() const
{
	return this->BACKWARD;
}

const DirectX::XMVECTOR& Camera::GetUpVector() const
{
	return this->UP;
}

const DirectX::XMVECTOR& Camera::GetLeftVector() const
{
	return this->LEFT;
}

const DirectX::XMVECTOR& Camera::GetRightVector() const
{
	return this->RIGHT;
}

void Camera::SetProjectionMatrix(float fovDegree, float aspectRatio, float nearZ, float farZ)
{
	float fovRadian = fovDegree * (DirectX::XM_PI / 180.0f);
	this->Projection_M = DirectX::XMMatrixPerspectiveFovLH(fovRadian, aspectRatio, nearZ, farZ);
}

void Camera::SetPosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = pos;
	DirectX::XMStoreFloat4(&this->Pos_F4, pos);
	this->UpdateViewMatrix();
}

void Camera::SetPosition(float x, float y, float z)
{
	this->Pos_F4 = DirectX::XMFLOAT4(x, y, z, 1.0f);
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = rot;
	DirectX::XMStoreFloat3(&this->Rot_F3, rot);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(float x, float y, float z)
{
	this->Rot_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->UpdateViewMatrix();
}

void Camera::UpdatePosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = DirectX::XMVectorAdd(this->Pos_V, pos);
	DirectX::XMStoreFloat4(&this->Pos_F4, this->Pos_V);
	this->UpdateViewMatrix();
}

void Camera::UpdatePosition(float x, float y, float z)
{
	this->Pos_F4.x += x;
	this->Pos_F4.y += y;
	this->Pos_F4.z += z;
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->UpdateViewMatrix();
}

void Camera::UpdateRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = DirectX::XMVectorAdd(this->Rot_V, rot);
	DirectX::XMStoreFloat3(&this->Rot_F3, this->Rot_V);
	this->UpdateViewMatrix();
}

void Camera::UpdateRotation(float x, float y, float z)
{
	this->Rot_F3.x += x;
	this->Rot_F3.y += y;
	this->Rot_F3.z += z;
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYawFromVector(this->Rot_V);

	//DirectX::XMVECTOR target = DirectX::XMVectorMultiply( this->FORWARD, rotation.r[2]);

	//DirectX::XMVECTOR upDir = this->UP;
	

	DirectX::XMVECTOR target = DirectX::XMVector3TransformCoord(this->FORWARD, rotation);

	target = DirectX::XMVectorAdd(this->Pos_V, target);

	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(this->UP, rotation);

	this->View_M = DirectX::XMMatrixLookAtLH(this->Pos_V, target, upDir);
}
