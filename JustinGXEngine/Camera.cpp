#include "Camera.h"

Camera::Camera()
{
	this->SetPosition(0.0f, 5.0f, -15.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->UpdateViewMatrix();
}

const DirectX::XMMATRIX& Camera::GetViewMatrix()
{
	this->UpdateViewMatrix();
	return this->View_M;
}

const DirectX::XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->Projection_M;
}

void Camera::SetProjectionMatrix(float fovDegree, float aspectRatio, float nearZ, float farZ)
{
	float fovRadian = fovDegree * (DirectX::XM_PI / 180.0f);
	this->Projection_M = DirectX::XMMatrixPerspectiveFovLH(fovRadian, aspectRatio, nearZ, farZ);
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYawFromVector(this->GetRotationVector());

	DirectX::XMVECTOR target = DirectX::XMVector3TransformCoord(this->GetForwardVector(), rotation);

	target = DirectX::XMVectorAdd(this->GetPositionVector(), target);

	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(this->GetUpVector(), rotation);

	this->View_M = DirectX::XMMatrixLookAtLH(this->GetPositionVector(), target, upDir);
}
