#include "Shaders.h"

HRESULT Shaders::VertexShader::Initialize(ID3D11Device* device, const char* filename, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, UINT byteWidth)
{
    HRESULT hr = S_OK;

    auto blob = load_binary_blob(filename);

    // Create the vertex Shader
    hr = device->CreateVertexShader(blob.data(), blob.size(), nullptr, this->Shader.GetAddressOf());

    // Create the Input Layout
    hr = device->CreateInputLayout(layoutDesc, numElements, blob.data(), blob.size(), this->InputLayout.GetAddressOf());

    // Create Constant Buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = byteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::VertexShader::Initialize(ID3D11Device* device, const void* shaderByteCode, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, UINT byteWidth)
{
    HRESULT hr = S_OK;

    // Create the vertex Shader
    hr = device->CreateVertexShader(shaderByteCode, sizeof(shaderByteCode), nullptr, this->Shader.GetAddressOf());

    // Create the Input Layout
    hr = device->CreateInputLayout(layoutDesc, numElements, shaderByteCode, sizeof(shaderByteCode), this->InputLayout.GetAddressOf());

    // Create Constant Buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = byteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

void Shaders::VertexShader::Bind(ID3D11DeviceContext* deviceContext)
{
    this->DeviceContext = deviceContext;
    if (this->ShaderConstantBuffer)
        this->DeviceContext->VSSetConstantBuffers(0, 1, this->ShaderConstantBuffer.GetAddressOf());
    if (this->InputLayout)
        this->DeviceContext->IASetInputLayout(this->InputLayout.Get());
    if (this->Shader)
        this->DeviceContext->VSSetShader(this->Shader.Get(), nullptr, 0);
}

const ID3D11VertexShader* Shaders::VertexShader::GetShader() const
{
    return this->Shader.Get();
}

const ID3D11InputLayout* Shaders::VertexShader::GetInputLayout() const
{
    return this->InputLayout.Get();
}

const ID3D11Buffer* Shaders::VertexShader::GetConstantBuffer() const
{
    return this->ShaderConstantBuffer.Get();
}

//
// Pixel Shader Functions ====================================================================
//

HRESULT Shaders::PixelShader::Initialize(ID3D11Device* device, const char* filename, UINT byteWidth)
{
    HRESULT hr = S_OK;

    auto blob = load_binary_blob(filename);

    // Create the vertex Shader
    hr = device->CreatePixelShader(blob.data(), blob.size(), nullptr, this->Shader.GetAddressOf());

    // Create Constant Buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = byteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::PixelShader::Initialize(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth)
{
    HRESULT hr = S_OK;

    // Create the vertex Shader
    hr = device->CreatePixelShader(shaderByteCode, sizeof(shaderByteCode), nullptr, this->Shader.GetAddressOf());

    // Create Constant Buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = byteWidth;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::PixelShader::InitShaderResources(ID3D11Device* device, std::string texFilename[2])
{
    HRESULT hr = S_OK;

    for (size_t i = 0; i < 2; i++)
    {
        std::wstring widestr = std::wstring(texFilename[i].begin(), texFilename[i].end());
        const wchar_t* widecstr = widestr.c_str();
        hr = DirectX::CreateDDSTextureFromFile(device, widecstr, nullptr, this->SRVs[i].GetAddressOf());
    }

    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    hr = device->CreateSamplerState(&sd, this->SamplerState.GetAddressOf());

    return hr;
}

HRESULT Shaders::PixelShader::Initialize_ALL(ID3D11Device* device, const char* filename, UINT byteWidth, std::string texFilename[2])
{
    HRESULT hr;
    hr = this->Initialize(device, filename, byteWidth);

    hr = this->InitShaderResources(device, texFilename);

    return hr;
}

HRESULT Shaders::PixelShader::Initialize_ALL(ID3D11Device* device, const void* shaderByteCode, UINT byteWidth, std::string texFilename[2])
{
    HRESULT hr;
    hr = this->Initialize(device, shaderByteCode, byteWidth);

    hr = this->InitShaderResources(device, texFilename);

    return hr;
}

void Shaders::PixelShader::Bind(ID3D11DeviceContext* deviceContext)
{
    this->DeviceContext = deviceContext;
    if (this->ShaderConstantBuffer)
        this->DeviceContext->PSSetConstantBuffers(0, 1, this->ShaderConstantBuffer.GetAddressOf());
    if (this->Shader)
        this->DeviceContext->PSSetShader(this->Shader.Get(), nullptr, 0);
}

void Shaders::PixelShader::BindShaderResources(ID3D11DeviceContext* deviceContext)
{
    this->DeviceContext = deviceContext;
    if (this->SRVs)
        this->DeviceContext->PSSetShaderResources(0, 2, this->SRVs->GetAddressOf());
    if (this->SamplerState)
        this->DeviceContext->PSSetSamplers(0, 1, this->SamplerState.GetAddressOf());
}

void Shaders::PixelShader::Bind_ALL(ID3D11DeviceContext* deviceContext)
{
    this->Bind(deviceContext);
    this->BindShaderResources(deviceContext);
}

const ID3D11PixelShader* Shaders::PixelShader::GetShader() const
{
    return this->Shader.Get();
}

const ID3D11Buffer* Shaders::PixelShader::GetConstantBuffer() const
{
    return this->ShaderConstantBuffer.Get();
}

const ID3D11ShaderResourceView* Shaders::PixelShader::GetShaderResourceView() const
{
    return this->ShaderResourceView.Get();
}

const ID3D11ShaderResourceView* Shaders::PixelShader::GetShaderResourceView_1() const
{
    return this->ShaderResourceView_1.Get();
}

const ID3D11SamplerState* Shaders::PixelShader::GetSamplerState() const
{
    return this->SamplerState.Get();
}
