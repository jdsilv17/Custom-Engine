#pragma once

#include "DDSTextureLoader.h"
#include <d3d11_1.h>
#include <wrl/client.h>
#include <string>
#include "BinaryFileLoad.h"


using Microsoft::WRL::ComPtr;

namespace Shaders
{
	class VertexShader
	{
	public:
		HRESULT Initialize(ID3D11Device* device, const char* filename,
			const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, UINT byteWidth);
		HRESULT Initialize(ID3D11Device* device, const void* shaderByteCode,
			const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, UINT byteWidth);
		void Bind(ID3D11DeviceContext* deviceContext);

		const ID3D11VertexShader* GetShader() const;
		const ID3D11InputLayout* GetInputLayout() const;
		const ID3D11Buffer* GetConstantBuffer() const;
		//const ID3D11Buffer* const* GetAddressOfConstantBuffer() const;

		//void SetConstantBuffer(const ID3D11Buffer* const* buffer);

		ComPtr<ID3D11Buffer> ShaderConstantBuffer = nullptr;

	private:
		ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
		ComPtr<ID3D11VertexShader> Shader = nullptr;
		ComPtr<ID3D11InputLayout> InputLayout = nullptr;

	};

	class PixelShader
	{
	public:
		// remake for use of multiple textures
		HRESULT Initialize(ID3D11Device* device, const char* filename, UINT byteWidth);
		HRESULT Initialize(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth);
		HRESULT InitShaderResources(ID3D11Device* device, std::string texFilename[2]);
		HRESULT InitShaderResources(ID3D11Device* device, std::string texFilename);
		HRESULT Initialize_ALL(ID3D11Device* device, const char* filename, UINT byteWidth, std::string texFilename[2]);
		HRESULT Initialize_ALL(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth, std::string texFilename[2]);
		void Bind(ID3D11DeviceContext* deviceContext);
		void BindShaderResources(ID3D11DeviceContext* deviceContext);
		void BindShaderResources_1(ID3D11DeviceContext* deviceContext);
		void Bind_ALL(ID3D11DeviceContext* deviceContext);

		const ID3D11PixelShader* GetShader() const;
		const ID3D11Buffer* GetConstantBuffer() const;
		//const ID3D11Buffer* const* GetAddressOfConstantBuffer() const;
		const ID3D11ShaderResourceView* GetShaderResourceView() const;
		const ID3D11ShaderResourceView* GetShaderResourceView_1() const;
		const ID3D11SamplerState* GetSamplerState() const;

		//void SetConstantBuffer(const ID3D11Buffer* const* buffer);
		
		ComPtr<ID3D11Buffer> ShaderConstantBuffer = nullptr;

	private:
		ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
		ComPtr<ID3D11PixelShader> Shader = nullptr;
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView = nullptr;
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView_1 = nullptr;
		ComPtr<ID3D11SamplerState> SamplerState = nullptr;
		//ComPtr<ID3D11Texture2D> Texture = nullptr;

		ComPtr<ID3D11ShaderResourceView> SRVs[2] =
		{
			ShaderResourceView,
			ShaderResourceView_1
		};

	};

	class GeometryShader
	{
	public:
		HRESULT Initialize(ID3D11Device* device, const char* filename, UINT byteWidth);
		HRESULT Initialize(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth);
		void Bind(ID3D11DeviceContext* deviceContext);

		const ID3D11GeometryShader* GetShader() const;
		const ID3D11InputLayout* GetInputLayout() const;
		const ID3D11Buffer* GetConstantBuffer() const;

		ComPtr<ID3D11Buffer> ShaderConstantBuffer = nullptr;

	private:
		ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
		ComPtr<ID3D11GeometryShader> Shader = nullptr;
		ComPtr<ID3D11InputLayout> InputLayout = nullptr;

	};
}
