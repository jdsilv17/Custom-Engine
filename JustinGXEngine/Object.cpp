#include "Object.h"

Object::Object()
{
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(0.0f, 0.0f, 0.0f);
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
	return this->FORWARD;
}

const DirectX::XMVECTOR& Object::GetBackwardVector() const
{
	return this->BACKWARD;
}

const DirectX::XMVECTOR& Object::GetUpVector() const
{
	return this->UP;
}

const DirectX::XMVECTOR& Object::GetLeftVector() const
{
	return this->LEFT;
}

const DirectX::XMVECTOR& Object::GetRightVector() const
{
	return this->RIGHT;
}

void Object::SetPosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = pos;
	DirectX::XMStoreFloat4(&this->Pos_F4, pos);
}

void Object::SetPosition(float x, float y, float z)
{
	this->Pos_F4 = DirectX::XMFLOAT4(x, y, z, 1.0f);
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
}

void Object::SetRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = rot;
	DirectX::XMStoreFloat3(&this->Rot_F3, rot);
}

void Object::SetRotation(float x, float y, float z)
{
	this->Rot_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
}

void Object::SetScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = scale;
	DirectX::XMStoreFloat3(&this->Scale_F3, scale);
}

void Object::SetScale(float x, float y, float z)
{
	this->Scale_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
}

void Object::UpdatePosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = DirectX::XMVectorAdd(this->Pos_V, pos);
	DirectX::XMStoreFloat4(&this->Pos_F4, this->Pos_V);
}

void Object::UpdatePosition(float x, float y, float z)
{
	this->Pos_F4.x += x;
	this->Pos_F4.y += y;
	this->Pos_F4.z += z;
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
}

void Object::UpdateRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = DirectX::XMVectorAdd(this->Rot_V, rot);
	DirectX::XMStoreFloat3(&this->Rot_F3, this->Rot_V);
}

void Object::UpdateRotation(float x, float y, float z)
{
	this->Rot_F3.x += x;
	this->Rot_F3.y += y;
	this->Rot_F3.z += z;
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
}

void Object::UpdateScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = DirectX::XMVectorAdd(this->Scale_V, scale);
	DirectX::XMStoreFloat3(&this->Scale_F3, this->Scale_V);
}

void Object::UpdateScale(float x, float y, float z)
{
	this->Scale_F3.x += x;
	this->Scale_F3.y += y;
	this->Scale_F3.z += z;
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
}

void Object::UpdateTransform()
{
	//this->Transform_F = DirectX::XMMatrixScaling;
}
