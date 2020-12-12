#include "Light.h"

Light::Light()
{
	this->SetLightColor(0.75f, 0.75f, 0.75f, 1.0f);
	this->SetAmbientTerm(0.3f);
}

const DirectX::XMVECTOR& Light::GetLightColorVector() const
{
	return this->Color_V;
}

const DirectX::XMFLOAT4& Light::GetLightColorFloat4() const
{
	return this->Color_F;
}

const float Light::GetAmbientTerm() const
{
	return this->AmbientTerm;
}

void Light::SetLightColor(const DirectX::XMVECTOR& color)
{
	this->Color_V = color;
	DirectX::XMStoreFloat4(&this->Color_F, this->Color_V);
}

void Light::SetLightColor(float r, float g, float b, float a)
{
	this->Color_F = DirectX::XMFLOAT4(r, g, b, a);
	this->Color_V = DirectX::XMLoadFloat4(&this->Color_F);
}

void Light::SetAmbientTerm(float ambient)
{
	this->AmbientTerm = ambient;
}

bool Light::IsEnabled()
{
	return this->Enabled;
}

bool Light::IsToggleable()
{
	return this->Toggleable;
}

//
// Directional LIght Class Functions ========================================================
//

DirectionalLight::DirectionalLight()
{
	this->SetDirection(-0.577f, 0.577f, -0.577f);
}

const DirectX::XMVECTOR& DirectionalLight::GetDirectionVector() const
{
	return this->Direction_V;
}

const DirectX::XMFLOAT3& DirectionalLight::GetDirectionFloat3() const
{
	return this->Direction_F;
}

void DirectionalLight::SetDirection(const DirectX::XMVECTOR& direction)
{
	this->Direction_V = direction;
	DirectX::XMStoreFloat3(&this->Direction_F, this->Direction_V);
}

void DirectionalLight::SetDirection(float x, float y, float z)
{
	this->Direction_F = DirectX::XMFLOAT3(x, y, z);
	this->Direction_V = DirectX::XMLoadFloat3(&this->Direction_F);
}

//
// Point Light Class Functions ==============================================================
//

PointLight::PointLight()
{
	this->SetPointRadius(35.0f);
}

const float PointLight::GetPointRadius() const
{
	return this->PointRadius;
}

void PointLight::SetPointRadius(float radius)
{
	this->PointRadius = radius;
}

// SpotLight Class Functions =================================================================

SpotLight::SpotLight()
{
	this->SetDirection(0.0f, -1.0f, 0.0f);

	this->SetOuterInnerConeRatios(0.5f, 0.8f);
}

const DirectX::XMVECTOR& SpotLight::GetConeDirectionVector() const
{
	return this->ConeDirection_V;
}

const DirectX::XMFLOAT3& SpotLight::GetConeDirectionFloat3() const
{
	return this->ConeDirection_F;
}

const float SpotLight::GetOuterConeRatio() const
{
	return this->OuterConeRatio;
}

const float SpotLight::GetInnerConeRatio() const
{
	return this->InnerConeRatio;
}

void SpotLight::SetDirection(const DirectX::XMVECTOR& coneDir)
{
	this->ConeDirection_V = coneDir;
	DirectX::XMStoreFloat3(&this->ConeDirection_F, this->ConeDirection_V);
}

void SpotLight::SetDirection(float x, float y, float z)
{
	this->ConeDirection_F = DirectX::XMFLOAT3(x, y, z);
	this->ConeDirection_V = DirectX::XMLoadFloat3(&this->ConeDirection_F);
}

void SpotLight::SetOuterConeRatio(float outerConeRatio)
{
	this->OuterConeRatio = outerConeRatio;
}

void SpotLight::SetInnerConeRatio(float innerConeRatio)
{
	this->InnerConeRatio = innerConeRatio;
}

void SpotLight::SetOuterInnerConeRatios(float outerConeRatio, float innerConeRatio)
{
	this->SetOuterConeRatio(outerConeRatio);
	this->SetInnerConeRatio(innerConeRatio);
}
