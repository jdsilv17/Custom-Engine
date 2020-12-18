#include "Camera.h"

Camera::Camera()
{
	this->SetPosition(0.0f, 1.0f, -3.0f);
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
	DirectX::XMMATRIX cameraRotation = DirectX::XMMatrixRotationRollPitchYawFromVector(this->GetRotationVector());
	// adjust directions for rotation
	DirectX::XMMATRIX directionRotation = DirectX::XMMatrixRotationRollPitchYaw(this->GetRotationFloat4().x, this->GetRotationFloat4().y, 0.0f);
	this->SetForwardVector(DirectX::XMVector3TransformCoord(this->FORWARD, directionRotation));
	this->SetBackwardVector(DirectX::XMVector3TransformCoord(this->BACKWARD, directionRotation));
	this->SetLeftVector(DirectX::XMVector3TransformCoord(this->LEFT, directionRotation));
	this->SetRightVector(DirectX::XMVector3TransformCoord(this->RIGHT, directionRotation));

	DirectX::XMVECTOR target = DirectX::XMVector3TransformCoord(this->FORWARD, cameraRotation);

	target = DirectX::XMVectorAdd(this->GetPositionVector(), target);

	DirectX::XMVECTOR upDir = DirectX::XMVector3TransformCoord(this->UP, cameraRotation);

	this->SetWorld(directionRotation * DirectX::XMMatrixTranslationFromVector(this->GetPositionVector()));
	this->View_M = DirectX::XMMatrixLookAtLH(this->GetPositionVector(), target, upDir);
}
