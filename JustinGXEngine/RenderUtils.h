#pragma once
//
// NOT READY FOR USE
//

#include <d3d11_1.h>
//#include <d3dcompiler.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

template <typename T>
HRESULT CreateConstantBuffer(ID3D11Device* device, UINT size, ComPtr<ID3D11Buffer>& constantBuffer)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = size;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    hr = device->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());

    return hr;
}

template <typename T>
HRESULT CreateVertexBuffer(ID3D11Device* device, int vertexCount, std::vector<T>& vertexList, ComPtr<ID3D11Buffer>& vertexBuffer)
{
	HRESULT hr = S_OK;
    auto v = sizeof(T);
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * vertexCount;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = vertexList.data();

    hr = device->CreateBuffer(&bd, &subData, vertexBuffer.GetAddressOf());

    return hr;
}

template <typename T>
HRESULT CreateVertexBuffer(ID3D11Device* device, int vertexCount, const T* vertexList, ComPtr<ID3D11Buffer>& vertexBuffer)
{
    HRESULT hr = S_OK;
    auto v = sizeof(T);
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * vertexCount;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = vertexList;

    hr = device->CreateBuffer(&bd, &subData, vertexBuffer.GetAddressOf());

    return hr;
}

template <typename T>
HRESULT CreateIndexBuffer(ID3D11Device* device, int indexCount, std::vector<T>& indicesList, ComPtr<ID3D11Buffer>& indexBuffer)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * indexCount;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = indicesList.data();

    hr = device->CreateBuffer(&bd, &subData, indexBuffer.GetAddressOf());

    return hr;
}

template <typename T>
HRESULT CreateIndexBuffer(ID3D11Device* device, int indexCount, const T* indicesList, ComPtr<ID3D11Buffer>& indexBuffer)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * indexCount;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = indicesList;

    hr = device->CreateBuffer(&bd, &subData, indexBuffer.GetAddressOf());

    return hr;
}