#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

typedef union JXMVECTOR
{
    DirectX::XMVECTOR v;

    DirectX::XMFLOAT4 fv;
}*PJXMVECTOR;

struct VERTEX
{
    DirectX::XMFLOAT4 pos = { 0, 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 0 };
    DirectX::XMFLOAT3 normals = { 0, 0, 0 };
    DirectX::XMFLOAT2 uv = { 0, 0 };

    VERTEX() {}
    VERTEX(const DirectX::XMFLOAT4& _pos)
        : pos(_pos) {}
    VERTEX(const DirectX::XMFLOAT3& _norm)
        : normals(_norm) {}
    VERTEX(const DirectX::XMFLOAT2& _uv)
        : uv(_uv) {}
    VERTEX(const DirectX::XMFLOAT4& _pos, const DirectX::XMFLOAT4& _col)
        : pos(_pos), color(_col) {}
    VERTEX(const DirectX::XMFLOAT4& _pos, const DirectX::XMFLOAT4& _col, const DirectX::XMFLOAT3& _norm, const DirectX::XMFLOAT2& _uv)
        : pos( _pos ), color(_col), normals(_norm), uv(_uv) {}
    VERTEX(float x, float y, float z, float w, float r, float g, float b, float a, float nx, float ny, float nz, float u, float v)
        : pos(x, y, z, w), color(r, g, b, a), normals(nx, ny, nz), uv(u, v) {}

};

struct COLORED_VERTEX
{
    DirectX::XMFLOAT4 pos = { 0, 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 0 };

    COLORED_VERTEX() {}
    COLORED_VERTEX(const DirectX::XMFLOAT4& _pos)
        : pos(_pos) {}
    COLORED_VERTEX(const DirectX::XMFLOAT4& _pos, const DirectX::XMFLOAT4& _col)
        : pos(_pos), color(_col) {}

};

struct VERTEX_TANGENT
{
    DirectX::XMFLOAT4 pos = { 0, 0, 0, 0 };
    DirectX::XMFLOAT3 normals = { 0, 0, 0 };
    DirectX::XMFLOAT4 tangent = { 0, 0, 0 ,0 };
    DirectX::XMFLOAT2 uv = { 0, 0 };

    VERTEX_TANGENT() {}
    VERTEX_TANGENT(const DirectX::XMFLOAT4& _pos)
        : pos(_pos) {}
    VERTEX_TANGENT(const DirectX::XMFLOAT3& _norm)
        : normals(_norm) {}
    VERTEX_TANGENT(const DirectX::XMFLOAT2& _uv)
        : uv(_uv) {}
    VERTEX_TANGENT(const DirectX::XMFLOAT4& _pos, const DirectX::XMFLOAT3& _norm, const DirectX::XMFLOAT4& _tang, const DirectX::XMFLOAT2& _uv)
        : pos(_pos), normals(_norm), tangent(_tang), uv(_uv) {}
    VERTEX_TANGENT(float x, float y, float z, float w, float nx, float ny, float nz, float tx, float ty, float tz, float tw, float u, float v)
        : pos(x, y, z, w), normals(nx, ny, nz), tangent(tx, ty, tz, tw), uv(u, v) {}

};

struct VERTEX_BASIC
{
    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT3 normals = { 0, 0, 0 };
    DirectX::XMFLOAT3 uvw = { 0, 0, 0 };

    VERTEX_BASIC() {}
    VERTEX_BASIC(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _norm, const DirectX::XMFLOAT3& _uvw)
        : pos(_pos), normals(_norm), uvw(_uvw) {}
    VERTEX_BASIC(float x, float y, float z, float nx, float ny, float nz, float u, float v, float w)
        : pos(x, y, z), normals(nx, ny, nz), uvw(u, v, w) {}
};

const D3D11_INPUT_ELEMENT_DESC vertexInputLayoutDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const D3D11_INPUT_ELEMENT_DESC coloredVertexLayoutDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const D3D11_INPUT_ELEMENT_DESC tangentInputLayoutDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const D3D11_INPUT_ELEMENT_DESC objLayoutDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC cubeLayoutDesc[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

std::vector<VERTEX> MakeGrid(float gridSize, int lineCount) 
{
    std::vector<VERTEX> lines;
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
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
        }
        else
        {
            // far point
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }));
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
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
        }
        else
        {
            // left point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, { 0.5f, 0.5f, 0.5f, 1.0f }));
        }
        // move over on the x-axis by the spacing
        z += lineSpacing;
    }

    return lines;
}

std::vector<VERTEX> MakeColorGrid(float gridSize, int lineCount, float deltaTime)
{
    std::vector<VERTEX> lines;

    // need: size, spacing, linecount, 
    int maxVerts = 2048;
    if (lineCount * 4 > maxVerts)
        lineCount = 510;

    float lineSpacing = gridSize / static_cast<float>(lineCount);

    float x = -gridSize / 2.0f; // starting point
    float z = -gridSize / 2.0f; // starting point


    DirectX::XMVECTOR zero = { 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMVECTOR one = { 1.0f, 1.0f, 1.0f, 1.0f };
    static DirectX::XMFLOAT4 color;
    static bool max = false;

    if (max)
    {
        DirectX::XMStoreFloat4(&color, DirectX::XMVectorLerp(zero, DirectX::XMLoadFloat4(&color), 0.1f ));
        if (color.x <= 0.0f && color.y <= 0.0f && color.z <= 0.0f)
            max = false;
    }
    else if (!max)
    {
        DirectX::XMStoreFloat4(&color, DirectX::XMVectorLerp(one, DirectX::XMLoadFloat4(&color), 0.5f ));
        if (color.x >= 1.0f && color.y >= 1.0f && color.z >= 1.0f)
            max = true;
    }


    // create lines along x-axis
    for (int i = 0; i <= lineCount; ++i)
    {
        // create the line
        if (i == lineCount / 2)
        {
            // far point
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
        }
        else
        {
            // far point
            lines.push_back(VERTEX({ x, 0.0f, z + gridSize, 1.0f }, color));
            // near point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, color));
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
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }));
        }
        else
        {
            // left point
            lines.push_back(VERTEX({ x, 0.0f, z, 1.0f }, color));
            // right point
            lines.push_back(VERTEX({ x + gridSize, 0.0f, z, 1.0f }, color));
        }
        // move over on the x-axis by the spacing
        z += lineSpacing;
    }

    return lines;
}