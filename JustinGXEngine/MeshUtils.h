#pragma once

#include <directxmath.h>

using namespace DirectX;

struct VERTEX_4
{
    XMFLOAT4 pos;
    //XMFLOAT4 color;
    XMFLOAT3 norm;
    XMFLOAT2 uv;
};