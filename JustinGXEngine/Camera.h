#pragma once

#include <DirectXMath.h>

class Camera
{
public:

	Camera();
	//~Camera();
	//Camera(const Camera& that);
	//Camera& operator=(const Camera& that);

	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetProjectionMatrix() const;
	const DirectX::XMVECTOR& GetPositionVector() const;
	const DirectX::XMFLOAT4& GetPositionFloat4() const;
	const DirectX::XMVECTOR& GetRotationVector() const;
	const DirectX::XMFLOAT3& GetRotationFloat4() const;
	const DirectX::XMVECTOR& GetForwardVector() const;
	const DirectX::XMVECTOR& GetBackwardVector() const;
	const DirectX::XMVECTOR& GetUpVector() const;
	const DirectX::XMVECTOR& GetLeftVector() const;
	const DirectX::XMVECTOR& GetRightVector() const;

	void SetProjectionMatrix(float fovDegree, float aspectRatio, float nearZ, float farZ);
	void SetPosition(const DirectX::XMVECTOR& pos);
	void SetPosition(float x, float y, float z);
	void SetRotation(const DirectX::XMVECTOR& rot);
	void SetRotation(float x, float y, float z);

	void UpdatePosition(const DirectX::XMVECTOR& pos);
	void UpdatePosition(float x, float y, float z);
	void UpdateRotation(const DirectX::XMVECTOR& rot);
	void UpdateRotation(float x, float y, float z);


private:
	DirectX::XMMATRIX View_M;
	DirectX::XMMATRIX Projection_M;

	DirectX::XMVECTOR Pos_V;
	DirectX::XMVECTOR Rot_V;
	DirectX::XMFLOAT4 Pos_F4;
	DirectX::XMFLOAT3 Rot_F3;

	const DirectX::XMVECTOR FORWARD = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const DirectX::XMVECTOR BACKWARD = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR LEFT = DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const DirectX::XMVECTOR RIGHT = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);


	void UpdateViewMatrix();
	
};

