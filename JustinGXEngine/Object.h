#pragma once

#include <DirectXMath.h>

class Object
{
public:
	Object();
	Object(const Object& that);
	Object& operator=(const Object& that);


	const DirectX::XMMATRIX& GetWorldMatrix() const;
	const DirectX::XMFLOAT4X4& GetWorldFloat4X4() const;
	// Transform
	const DirectX::XMMATRIX& GetTransformMatrix() const;
	const DirectX::XMFLOAT4X4& GetTransformFloat4X4() const;
	// Postion
	const DirectX::XMVECTOR& GetPositionVector() const;
	const DirectX::XMFLOAT4& GetPositionFloat4() const;
	// Rotation
	const DirectX::XMVECTOR& GetRotationVector() const;
	const DirectX::XMFLOAT3& GetRotationFloat4() const;
	// Scale
	const DirectX::XMVECTOR& GetScaleVector() const;
	const DirectX::XMFLOAT3& GetScaleFloat4() const;
	// Directions
	const DirectX::XMVECTOR& GetForwardVector() const;
	const DirectX::XMFLOAT3& GetForwardFloat3() const;
	const DirectX::XMVECTOR& GetBackwardVector() const;
	const DirectX::XMFLOAT3& GetBackwardFloat3() const;
	const DirectX::XMVECTOR& GetUpVector() const;
	const DirectX::XMFLOAT3& GetUpFloat3() const;
	const DirectX::XMVECTOR& GetLeftVector() const;
	const DirectX::XMFLOAT3& GetLeftFloat3() const;
	const DirectX::XMVECTOR& GetRightVector() const;
	const DirectX::XMFLOAT3& GetRightFloat3() const;


	void SetWorld(const DirectX::XMMATRIX& mat);
	void SetWorld(DirectX::XMFLOAT4X4 mat);

	void SetLookAt(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& target, const DirectX::XMVECTOR& up);
	void SetLookAt(const DirectX::XMVECTOR& zAxis, const DirectX::XMVECTOR& up);

	void SetTurnTo(const DirectX::XMMATRIX& mat, const DirectX::XMVECTOR& target, const float& deltaTime);
	void SetTurnTo(DirectX::XMFLOAT4X4 mat);
	// Transform
	void SetTransform(const DirectX::XMMATRIX& mat);
	void SetTransform(DirectX::XMFLOAT4X4 mat);
	// Postion
	void SetPosition(const DirectX::XMVECTOR& pos);
	void SetPosition(float x, float y, float z);
	// Rotation
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(float x, float y, float z);
	// Scale
	void SetScale(const DirectX::XMVECTOR& scale);
	void SetScale(float x, float y, float z);

	void SetForwardVector(const DirectX::XMVECTOR& forward);
	void SetBackwardVector(const DirectX::XMVECTOR& backward);
	void SetUpVector(const DirectX::XMVECTOR& up);
	void SetLeftVector(const DirectX::XMVECTOR& left);
	void SetRightVector(const DirectX::XMVECTOR& right);

	// Postion
	void UpdatePosition(const DirectX::XMVECTOR& pos);
	void UpdatePosition(float x, float y, float z);
	// Rotation
	void UpdateRotation(const DirectX::XMVECTOR& rot);
	void UpdateRotation(float x, float y, float z);
	// Scale
	void UpdateScale(const DirectX::XMVECTOR& scale);
	void UpdateScale(float x, float y, float z);

	const DirectX::XMVECTOR FORWARD = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR BACKWARD = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR LEFT = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR RIGHT = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

private:
	DirectX::XMMATRIX World_M;
	DirectX::XMFLOAT4X4 World_F;

	DirectX::XMMATRIX LookAt_M;
	DirectX::XMFLOAT4X4 LookAt_F;

	DirectX::XMMATRIX TurnTo_M;
	DirectX::XMFLOAT4X4 turnTo_F;

	DirectX::XMMATRIX Transform_M; 
	DirectX::XMFLOAT4X4 Transform_F;

	DirectX::XMVECTOR Pos_V; 
	DirectX::XMFLOAT4 Pos_F4;
	DirectX::XMVECTOR Rot_V; 
	DirectX::XMFLOAT3 Rot_F3;
	DirectX::XMVECTOR Scale_V; 
	DirectX::XMFLOAT3 Scale_F3;

	DirectX::XMVECTOR foward_V;
	DirectX::XMFLOAT3 foward_F3;
	DirectX::XMVECTOR backward_V;
	DirectX::XMFLOAT3 backward_F3;
	DirectX::XMVECTOR up_V;
	DirectX::XMFLOAT3 up_F3;
	DirectX::XMVECTOR left_V;
	DirectX::XMFLOAT3 left_F3;
	DirectX::XMVECTOR right_V;
	DirectX::XMFLOAT3 right_F3;

	void UpdateWorldMatrix();
	void UpdateTransform();
};
