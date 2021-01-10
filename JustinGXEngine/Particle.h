#pragma once
#include "Mesh.h"
class Particle /*:
    public Mesh<VERTEX>*/
{
public:
    Mesh<VERTEX> Mesh;

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

