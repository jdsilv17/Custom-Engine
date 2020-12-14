#pragma once

#include "DDSTextureLoader.h"
#include <d3d11_1.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <fstream>

using Microsoft::WRL::ComPtr;

namespace Shaders
{

	static std::vector<uint8_t> load_binary_blob(const char* path)
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

	private:
		ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
		ComPtr<ID3D11VertexShader> Shader = nullptr;
		ComPtr<ID3D11InputLayout> InputLayout = nullptr;
		ComPtr<ID3D11Buffer> ShaderConstantBuffer = nullptr;

	};

	class PixelShader
	{
	public:
		// remake for use of multiple textures
		HRESULT Initialize(ID3D11Device* device, const char* filename, UINT byteWidth);
		HRESULT Initialize(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth);
		HRESULT InitShaderResources(ID3D11Device* device, std::string texFilename[2]);
		HRESULT Initialize_ALL(ID3D11Device* device, const char* filename, UINT byteWidth, std::string texFilename[2]);
		HRESULT Initialize_ALL(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth, std::string texFilename[2]);
		void Bind(ID3D11DeviceContext* deviceContext);
		void BindShaderResources(ID3D11DeviceContext* deviceContext);
		void Bind_ALL(ID3D11DeviceContext* deviceContext);

		const ID3D11PixelShader* GetShader() const;
		const ID3D11Buffer* GetConstantBuffer() const;
		const ID3D11ShaderResourceView* GetShaderResourceView() const;
		const ID3D11ShaderResourceView* GetShaderResourceView_1() const;
		const ID3D11SamplerState* GetSamplerState() const;

	private:
		ComPtr<ID3D11DeviceContext> DeviceContext = nullptr;
		ComPtr<ID3D11PixelShader> Shader = nullptr;
		ComPtr<ID3D11Buffer> ShaderConstantBuffer = nullptr;
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView;
		ComPtr<ID3D11ShaderResourceView> ShaderResourceView_1;
		ComPtr<ID3D11SamplerState> SamplerState = nullptr;

		ComPtr<ID3D11ShaderResourceView> SRVs[2] =
		{
			ShaderResourceView,
			ShaderResourceView_1
		};

	};
}
