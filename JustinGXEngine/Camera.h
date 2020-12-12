#pragma once

#include "Object.h"
#include <DirectXMath.h>

class Camera : 
	public Object
{
public:

	Camera();
	//~Camera();
	//Camera(const Camera& that);
	//Camera& operator=(const Camera& that);

	//const DirectX::XMMATRIX& UpdateViewMatrix() const;

	//const DirectX::XMMATRIX& GetWorldMatrix() const;
	const DirectX::XMMATRIX& GetViewMatrix();
	const DirectX::XMMATRIX& GetProjectionMatrix() const;

	//void SetWorldMatrix(const DirectX::XMMATRIX& mat);
	void SetProjectionMatrix(float fovDegree, float aspectRatio, float nearZ, float farZ);


private:
	//DirectX::XMMATRIX World_M;
	DirectX::XMMATRIX View_M;
	DirectX::XMMATRIX Projection_M;

	void UpdateViewMatrix();
	
};

