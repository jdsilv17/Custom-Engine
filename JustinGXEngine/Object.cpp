#include "Object.h"

Object::Object()
{
	this->SetWorld(DirectX::XMMatrixIdentity());
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(1.0f, 1.0f, 1.0f);
}

const DirectX::XMMATRIX& Object::GetWorldMatrix() const
{
	return this->World_M;
}

const DirectX::XMFLOAT4X4& Object::GetWorldFloat4X4() const
{
	return this->World_F;
}

const DirectX::XMVECTOR& Object::GetPositionVector() const
{
	return this->Pos_V;
}

const DirectX::XMFLOAT4& Object::GetPositionFloat4() const
{
	return this->Pos_F4;
}

const DirectX::XMVECTOR& Object::GetRotationVector() const
{
	return this->Rot_V;
}

const DirectX::XMFLOAT3& Object::GetRotationFloat4() const
{
	return this->Rot_F3;
}

const DirectX::XMVECTOR& Object::GetScaleVector() const
{
	return this->Scale_V;
}

const DirectX::XMFLOAT3& Object::GetScaleFloat4() const
{
	return this->Scale_F3;
}

const DirectX::XMVECTOR& Object::GetForwardVector() const
{
	return this->foward_V;
}

const DirectX::XMVECTOR& Object::GetBackwardVector() const
{
	return this->backward_V;
}

const DirectX::XMVECTOR& Object::GetUpVector() const
{
	return this->UP;
}

const DirectX::XMVECTOR& Object::GetLeftVector() const
{
	return this->left_V;
}

const DirectX::XMVECTOR& Object::GetRightVector() const
{
	return this->right_V;
}

void Object::SetWorld(const DirectX::XMMATRIX& mat)
{
	this->World_M = mat;
	DirectX::XMStoreFloat4x4(&this->World_F, this->World_M);
}

void Object::SetWorld(DirectX::XMFLOAT4X4 mat)
{
	this->World_F = mat;
	this->World_M = DirectX::XMLoadFloat4x4(&this->World_F);
}

void Object::SetPosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = pos;
	DirectX::XMStoreFloat4(&this->Pos_F4, pos);
	this->UpdateWorldMatrix();
}

void Object::SetPosition(float x, float y, float z)
{
	this->Pos_F4 = DirectX::XMFLOAT4(x, y, z, 1.0f);
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->UpdateWorldMatrix();
}

void Object::SetRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = rot;
	DirectX::XMStoreFloat3(&this->Rot_F3, rot);
	this->UpdateWorldMatrix();
}

void Object::SetRotation(float x, float y, float z)
{
	this->Rot_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->UpdateWorldMatrix();
}

void Object::SetScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = scale;
	DirectX::XMStoreFloat3(&this->Scale_F3, scale);
	this->UpdateWorldMatrix();
}

void Object::SetScale(float x, float y, float z)
{
	this->Scale_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
	this->UpdateWorldMatrix();
}

void Object::SetForwardVector(const DirectX::XMVECTOR& forward)
{
	this->foward_V = forward;
}

void Object::SetBackwardVector(const DirectX::XMVECTOR& backward)
{
	this->backward_V = backward;
}

void Object::SetLeftVector(const DirectX::XMVECTOR& left)
{
	this->left_V = left;
}

void Object::SetRightVector(const DirectX::XMVECTOR& right)
{
	this->right_V = right;
}

void Object::UpdateWorldMatrix()
{
	DirectX::XMMATRIX translation = DirectX::XMMatrixTranslationFromVector(this->GetPositionVector());
	DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(this->GetRotationFloat4().x);
	DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(this->GetRotationFloat4().y);
	DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(this->GetRotationFloat4().z);
	DirectX::XMMATRIX rotation = rotationZ * rotationX * rotationY;
	DirectX::XMMATRIX scale = DirectX::XMMatrixScalingFromVector(this->GetScaleVector());

	DirectX::XMMATRIX world = scale * rotation * translation;
	this->SetWorld(world);
}

void Object::UpdatePosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = DirectX::XMVectorAdd(this->Pos_V, pos);
	DirectX::XMStoreFloat4(&this->Pos_F4, this->Pos_V);
	this->UpdateWorldMatrix();
}

void Object::UpdatePosition(float x, float y, float z)
{
	this->Pos_F4.x += x;
	this->Pos_F4.y += y;
	this->Pos_F4.z += z;
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->UpdateWorldMatrix();
}

void Object::UpdateRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = DirectX::XMVectorAdd(this->Rot_V, rot);
	DirectX::XMStoreFloat3(&this->Rot_F3, this->Rot_V);
	this->UpdateWorldMatrix();
}

void Object::UpdateRotation(float x, float y, float z)
{
	this->Rot_F3.x += x;
	this->Rot_F3.y += y;
	this->Rot_F3.z += z;
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->UpdateWorldMatrix();
}

void Object::UpdateScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = DirectX::XMVectorAdd(this->Scale_V, scale);
	DirectX::XMStoreFloat3(&this->Scale_F3, this->Scale_V);
	this->UpdateWorldMatrix();
}

void Object::UpdateScale(float x, float y, float z)
{
	this->Scale_F3.x += x;
	this->Scale_F3.y += y;
	this->Scale_F3.z += z;
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
	this->UpdateWorldMatrix();
}

void Object::UpdateTransform()
{
	//this->Transform_F = DirectX::XMMatrixScaling;
}
