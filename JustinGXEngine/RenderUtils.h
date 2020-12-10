#pragma once
#ifndef RenderUtils_h_
#define RenderUtils_h_
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
HRESULT CreateVertexBuffer(ID3D11Device* device, int vertexCount, T vertexList, ComPtr<ID3D11Buffer>& vertexBuffer)
{
	HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * vertexCount;
    bd.Usage = D3D11_USAGE_DEFAULT;
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
HRESULT CreateIndexBuffer(ID3D11Device* device, int indexCount, T indicesList, ComPtr<ID3D11Buffer>& indexBuffer)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(int) * indexCount;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = indicesList;

    hr = device->CreateBuffer(&bd, &subData, indexBuffer.GetAddressOf());

    return hr;
}

//template <typename T>
//void Set_Pipeline(ID3D11DeviceContext* immediateContext)
//{
//    Set_IA_Stage(immediateContext);
//    Set_VS_Stage(immediateContext);
//    Set_RS_Stage(immediateContext);
//    Set_PS_Stage(immediateContext);
//    Set_OM_Stage(immediateContext);
//}
//
//template <typename T>
//void Set_IA_Stage(ID3D11DeviceContext* immediateContext)
//{
//    UINT stride = sizeof(T);
//    UINT offset = 0;
//
//    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    immediateContext->IASetVertexBuffers(0, 1, meshVB, &stride, &offset);
//    immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//    immediateContext->IASetInputLayout(vertexMeshLayout);
//}
//
//template <typename T>
//void Set_VS_Stage(ID3D11DeviceContext* immediateContext)
//{
//    immediateContext->VSSetShader(vShader, 0, 0);
//}
//
//template <typename T>
//void Set_RS_Stage(ID3D11DeviceContext* immediateContext)
//{
//    immediateContext->RSSetViewports(1, &vPort);
//}
//
//template <typename T>
//void Set_PS_Stage(ID3D11DeviceContext* immediateContext)
//{
//    immediateContext->PSSetSamplers(startslot, numsamplers, ppSamplers);
//    immediateContext->PSSetShader(pShader, 0, 0);
//}
//
//template <typename T>
//void Set_OM_Stage(ID3D11DeviceContext* immediateContext)
//{
//    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);
//}

#endif // RenderUtils_h_