#pragma once
#include "Mesh.h"
class Particle :
    public Mesh<COLORED_VERTEX>
{
public:
    COLORED_VERTEX vertex;
    DirectX::XMFLOAT4 prev_pos;

    float liftime = 0;

private:

};

