#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

struct VERTEX
{
    DirectX::XMFLOAT4 pos = { 0, 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 0 };
    DirectX::XMFLOAT3 norm = { 0, 0, 0 };
    DirectX::XMFLOAT2 uv = { 0, 0 };

    VERTEX() {}
    VERTEX(float x, float y, float z, float w, float r, float g, float b, float a, float nx, float ny, float nz, float u, float v)
        : pos(x, y, z, w), color(r, g, b, a), norm(nx, ny, nz), uv(u, v) {}
    VERTEX(DirectX::XMFLOAT4 _pos, DirectX::XMFLOAT4 _col, DirectX::XMFLOAT3 _norm, DirectX::XMFLOAT2 _uv)
        : pos( _pos ), color(_col), norm(_norm), uv(_uv) {}

};

D3D11_INPUT_ELEMENT_DESC vertexInputLayout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

D3D11_INPUT_ELEMENT_DESC objLayout[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

std::vector<VERTEX> lines;
void MakeGrid(float gridSize, int lineCount) 
{
    // need: size, spacing, linecount, 
    float lineSpacing = gridSize / static_cast<float>(lineCount);

    float x = -gridSize / 2.0f; // starting point
    float z = -gridSize / 2.0f; // starting point

    // create lines along x-axis
    for (int i = 0; i <= lineCount; ++i)
    {
        // create the line
        if (i == lineCount / 2)
        {
            // far point
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
        }
        else
        {
            // far point
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
        }
        // move over on the x-axis by the spacing
        x += lineSpacing;
    }
    x = -gridSize / 2.0f;
    // create lines along z-axis
    for (int i = 0; i <= lineCount; ++i)
    {
        // create the line
        if (i == lineCount / 2)
        {
            // left point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
        }
        else
        {
            // left point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }, { 0, 0, 0 }, { 0, 0 }));
        }
        // move over on the x-axis by the spacing
        z += lineSpacing;
    }
}