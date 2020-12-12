#pragma once
#include "Object.h"
#include <DirectXMath.h>


enum LightTypes
{
    Directional,
    Point,
    Spot
};

class DirectionalLight;
class PointLight;
class SpotLight;

class Light :
    public Object
{
public:
    Light();



    const DirectX::XMVECTOR& GetLightColorVector() const; 
    const DirectX::XMFLOAT4& GetLightColorFloat4() const;

    const float GetAmbientTerm() const;

    void SetLightColor(const DirectX::XMVECTOR& color);
    void SetLightColor(float r, float g, float b, float a);

    void SetAmbientTerm(float ambient);

    bool IsEnabled();
    bool IsToggleable();

private:
    DirectX::XMVECTOR Color_V;
    DirectX::XMFLOAT4 Color_F;

    float AmbientTerm = 0.0f;

    bool Enabled = true;
    bool Toggleable = true;
};

class DirectionalLight :
    public Light
{
public:
    DirectionalLight();

    const DirectX::XMVECTOR& GetDirectionVector() const;
    const DirectX::XMFLOAT3& GetDirectionFloat3() const;

    void SetDirection(const DirectX::XMVECTOR& direction);
    void SetDirection(float x, float y, float z);

private:
    DirectX::XMVECTOR Direction_V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMFLOAT3 Direction_F = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    LightTypes LightType = Directional;
};

class PointLight :
    public Light
{
public:
    PointLight();

    const float GetPointRadius() const;

    void SetPointRadius(float radius);

private:
    float PointRadius = 0.0f;

    LightTypes LightType = Point;
};

class SpotLight :
    public Light
{
public:
    SpotLight();

    const DirectX::XMVECTOR& GetConeDirectionVector() const;
    const DirectX::XMFLOAT3& GetConeDirectionFloat3() const;

    const float GetOuterConeRatio() const;
    const float GetInnerConeRatio() const;

    void SetDirection(const DirectX::XMVECTOR& coneDir);
    void SetDirection(float x, float y, float z);

    void SetOuterConeRatio(float outerConeRatio);
    void SetInnerConeRatio(float innerConeRatio);
    void SetOuterInnerConeRatios(float outerConeRatio, float innerConeRatio);

private:
    DirectX::XMVECTOR ConeDirection_V = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    DirectX::XMFLOAT3 ConeDirection_F = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    float OuterConeRatio = 0.0f;
    float InnerConeRatio = 0.0f;

    LightTypes LightType = Spot;
};
