#include "Object.h"

Object::Object()
{
	this->SetWorld(DirectX::XMMatrixIdentity());
	this->SetPosition(0.0f, 0.0f, 0.0f);
	this->SetRotation(0.0f, 0.0f, 0.0f);
	this->SetScale(1.0f, 1.0f, 1.0f);
	this->SetForwardVector(this->FORWARD);
	this->SetBackwardVector(this->BACKWARD);
	this->SetUpVector(this->UP);
	this->SetLeftVector(this->LEFT);
	this->SetRightVector(this->RIGHT);
}

Object::Object(const DirectX::XMMATRIX& _world)
{
	this->SetWorld(_world);
}

Object::Object(const DirectX::XMFLOAT4X4& _world)
{
	this->SetWorld(_world);
}

Object::Object(const Object& that)
{
	*this = that;
}

Object& Object::operator=(const Object& that)
{
	if (this != &that)
	{
		this->SetPosition(that.Pos_V);
		this->SetRotation(that.Rot_V);
		this->SetScale(that.Scale_V);
		this->SetForwardVector(that.foward_V);
		this->SetBackwardVector(that.backward_V);
		this->SetLeftVector(that.left_V);
		this->SetRightVector(that.right_V);
		this->SetWorld(that.World_F);
		//this->SetTransform(that.Transform_F);
	}
	return *this;
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

const DirectX::XMFLOAT3& Object::GetForwardFloat3() const
{
	return this->foward_F3;
}

const DirectX::XMVECTOR& Object::GetBackwardVector() const
{
	return this->backward_V;
}

const DirectX::XMFLOAT3& Object::GetBackwardFloat3() const
{
	return this->backward_F3;
}

const DirectX::XMVECTOR& Object::GetUpVector() const
{
	return this->up_V;
}

const DirectX::XMFLOAT3& Object::GetUpFloat3() const
{
	return this->up_F3;
}

const DirectX::XMVECTOR& Object::GetLeftVector() const
{
	return this->left_V;
}

const DirectX::XMFLOAT3& Object::GetLeftFloat3() const
{
	return this->left_F3;
}

const DirectX::XMVECTOR& Object::GetRightVector() const
{
	return this->right_V;
}

const DirectX::XMFLOAT3& Object::GetRightFloat3() const
{
	return this->right_F3;
}

void Object::SetWorld(const DirectX::XMMATRIX& mat)
{
	this->World_M = mat;
	DirectX::XMStoreFloat4x4(&this->World_F, this->World_M);
	this->Pos_V = { this->World_F.m[3][0], this->World_F.m[3][1], this->World_F.m[3][2], this->World_F.m[3][3] };
	DirectX::XMStoreFloat4(&this->Pos_F4, this->Pos_V);
}

void Object::SetWorld(DirectX::XMFLOAT4X4 mat)
{
	this->World_F = mat;
	this->World_M = DirectX::XMLoadFloat4x4(&this->World_F);
	this->Pos_V = { this->World_F.m[3][0], this->World_F.m[3][1], this->World_F.m[3][2], this->World_F.m[3][3] };
	DirectX::XMStoreFloat4(&this->Pos_F4, this->Pos_V);
}

// move to math eventually
void Object::SetLookAt(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up)
{
	DirectX::XMVECTOR Z = DirectX::XMVector4Normalize(DirectX::XMVectorSubtract(target, position));
	DirectX::XMVECTOR X = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(up, Z));
	DirectX::XMVECTOR Y = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(Z, X));
	DirectX::XMMATRIX LookAt = {	X,
									Y,
									Z,
									this->GetPositionVector() };
	this->SetWorld(LookAt);
}

// move to math eventually
void Object::SetLookAt(const DirectX::XMVECTOR& zAxis, const DirectX::XMVECTOR& up)
{
	DirectX::XMVECTOR Z = DirectX::XMVector4Normalize(zAxis);
	DirectX::XMVECTOR X = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(up, Z));
	DirectX::XMVECTOR Y = DirectX::XMVector4Normalize(DirectX::XMVector3Cross(Z, X));
	DirectX::XMMATRIX LookAt = { X,
									Y,
									Z,
									this->GetPositionVector() };
	this->SetWorld(LookAt);
}

// move to math eventually
void Object::SetTurnTo(const DirectX::XMMATRIX& mat, const DirectX::XMVECTOR& target, const float& deltaTime)
{
	DirectX::XMVECTOR Z = DirectX::XMVector4Normalize(DirectX::XMVectorSubtract(target, mat.r[3]));

	DirectX::XMVECTOR dotX = DirectX::XMVector3Dot(Z, mat.r[0]);
	DirectX::XMVECTOR dotY = DirectX::XMVector3Dot(Z, mat.r[1]);
	// turn right
	float rot_Y_deg = dotX.m128_f32[0] * deltaTime /** (DirectX::XM_PI / 180.0f)*/;	// get yaw angle
	float rot_X_deg = dotY.m128_f32[0] * deltaTime /** (DirectX::XM_PI / 180.0f)*/; // get pitch angle

	// after turn, pass z to lookat algo to orthonormalize it
	DirectX::XMMATRIX TurnTo = DirectX::XMMatrixRotationX(-rot_X_deg) * DirectX::XMMatrixRotationY(rot_Y_deg) * mat;
	this->SetLookAt(TurnTo.r[2], this->UP);
}

void Object::SetPosition(const DirectX::XMVECTOR& pos)
{
	this->Pos_V = pos;
	DirectX::XMStoreFloat4(&this->Pos_F4, pos);
	this->PositionChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetPosition(float x, float y, float z)
{
	this->Pos_F4 = DirectX::XMFLOAT4(x, y, z, 1.0f);
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->PositionChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = rot;
	DirectX::XMStoreFloat3(&this->Rot_F3, rot);
	this->RotationChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetRotation(float x, float y, float z)
{
	this->Rot_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->RotationChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = scale;
	DirectX::XMStoreFloat3(&this->Scale_F3, scale);
	this->ScaleChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetScale(float x, float y, float z)
{
	this->Scale_F3 = DirectX::XMFLOAT3(x, y, z);
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
	this->ScaleChanged = true;
	this->UpdateWorldMatrix();
}

void Object::SetForwardVector(const DirectX::XMVECTOR& forward)
{
	this->foward_V = forward;
	DirectX::XMStoreFloat3(&this->foward_F3, this->foward_V);
}

void Object::SetBackwardVector(const DirectX::XMVECTOR& backward)
{
	this->backward_V = backward;
	DirectX::XMStoreFloat3(&this->backward_F3, this->backward_V);
}

void Object::SetUpVector(const DirectX::XMVECTOR& up)
{
	this->up_V = up;
	DirectX::XMStoreFloat3(&this->up_F3, this->up_V);
}

void Object::SetLeftVector(const DirectX::XMVECTOR& left)
{
	this->left_V = left;
	DirectX::XMStoreFloat3(&this->left_F3, this->left_V);
}

void Object::SetRightVector(const DirectX::XMVECTOR& right)
{
	this->right_V = right;
	DirectX::XMStoreFloat3(&this->right_F3, this->right_V);
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
	this->PositionChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdatePosition(float x, float y, float z)
{
	this->Pos_F4.x += x;
	this->Pos_F4.y += y;
	this->Pos_F4.z += z;
	this->Pos_V = DirectX::XMLoadFloat4(&this->Pos_F4);
	this->PositionChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdateRotation(const DirectX::XMVECTOR& rot)
{
	this->Rot_V = DirectX::XMVectorAdd(this->Rot_V, rot);
	DirectX::XMStoreFloat3(&this->Rot_F3, this->Rot_V);
	this->RotationChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdateRotation(float x, float y, float z)
{
	this->Rot_F3.x += x;
	this->Rot_F3.y += y;
	this->Rot_F3.z += z;
	this->Rot_V = DirectX::XMLoadFloat3(&this->Rot_F3);
	this->RotationChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdateScale(const DirectX::XMVECTOR& scale)
{
	this->Scale_V = DirectX::XMVectorAdd(this->Scale_V, scale);
	DirectX::XMStoreFloat3(&this->Scale_F3, this->Scale_V);
	this->ScaleChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdateScale(float x, float y, float z)
{
	this->Scale_F3.x += x;
	this->Scale_F3.y += y;
	this->Scale_F3.z += z;
	this->Scale_V = DirectX::XMLoadFloat3(&this->Scale_F3);
	this->ScaleChanged = true;
	this->UpdateWorldMatrix();
}

void Object::UpdateTransform()
{
	//this->Transform_F = DirectX::XMMatrixScaling;
}
