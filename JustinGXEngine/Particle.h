#pragma once
//#include <DirectXMath.h>
#include "Object.h"
class Particle /*:
    Object*/
{
public:
    float p;
    DirectX::XMFLOAT4 Pos;
    DirectX::XMFLOAT4 Color;
    DirectX::XMFLOAT4 prev_pos;
    DirectX::XMVECTOR Velocity;

    DirectX::XMVECTOR Gravity;
    float Lifetime;

    Particle();
    Particle(const Particle& that);
    Particle& operator=(const Particle& that);

private:

};

