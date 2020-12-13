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
#include <fstream>
#include <vector>

using Microsoft::WRL::ComPtr;

std::vector<uint8_t> load_binary_blob(const char* path)
{
    std::vector<uint8_t> blob;

    std::fstream file{ path, std::ios_base::in | std::ios_base::binary };

    if (file.is_open())
    {
        file.seekg(0, std::ios_base::end);
        blob.resize(file.tellg());
        file.seekg(0, std::ios_base::beg);

        file.read((char*)blob.data(), blob.size());

        file.close();
    }

    return std::move(blob);
}

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
    bd.Usage = D3D11_USAGE_DEFAULT;
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
HRESULT CreateIndexBuffer(ID3D11Device* device, int indexCount, std::vector<T>& indicesList, ComPtr<ID3D11Buffer>& indexBuffer)
{
    HRESULT hr = S_OK;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(T) * indexCount;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = indicesList.data();

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