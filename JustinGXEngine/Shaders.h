#pragma once

#include <d3d11_1.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class VertexShader
{
public:

private:
	ComPtr<ID3D11VertexShader> shader;
	ComPtr<ID3D10Blob> shader_buffer;
	ComPtr<ID3D11InputLayout> inputLayout;

};

class PixelShader
{
public:

private:

};

