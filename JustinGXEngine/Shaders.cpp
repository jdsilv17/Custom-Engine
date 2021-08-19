#include "Shaders.h"

HRESULT Shaders::VertexShader::Initialize(ID3D11Device* device, const char* filename, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    auto blob = load_binary::load_binary_blob(filename);

    // Create the vertex Shader
    hr = device->CreateVertexShader(blob.data(), blob.size(), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr))
        return hr;

    // Create the Input Layout
    hr = device->CreateInputLayout(layoutDesc, numElements, blob.data(), blob.size(), this->InputLayout.GetAddressOf());
    if (FAILED(hr))
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::VertexShader::Initialize(ID3D11Device* device, const void* shaderByteCode, const D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    // Create the vertex Shader
    hr = device->CreateVertexShader(shaderByteCode, sizeof(shaderByteCode), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr)) 
        return hr;

    // Create the Input Layout
    hr = device->CreateInputLayout(layoutDesc, numElements, shaderByteCode, sizeof(shaderByteCode), this->InputLayout.GetAddressOf());
    if (FAILED(hr)) 
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

void Shaders::VertexShader::Bind(ID3D11DeviceContext* deviceContext)
{
    if (this->DeviceContext)
    {
        this->DeviceContext.Reset();
        this->DeviceContext = deviceContext;
    }
    else
        this->DeviceContext = deviceContext;
    if (this->ShaderConstantBuffer)
        this->DeviceContext->VSSetConstantBuffers(0, 1, this->ShaderConstantBuffer.GetAddressOf());
    if (this->InputLayout)
        this->DeviceContext->IASetInputLayout(this->InputLayout.Get());
    if (this->Shader)
        this->DeviceContext->VSSetShader(this->Shader.Get(), nullptr, 0);
    this->DeviceContext.Reset();
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

HRESULT Shaders::PixelShader::Initialize(ID3D11Device* device, const char* filename, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    auto blob = load_binary::load_binary_blob(filename);

    // Create the vertex Shader
    hr = device->CreatePixelShader(blob.data(), blob.size(), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr))
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::PixelShader::Initialize(ID3D11Device* device, const void* shaderByteCode, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    // Create the vertex Shader
    hr = device->CreatePixelShader(shaderByteCode, sizeof(shaderByteCode), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr))
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

/// <summary>
/// Init Shaders for binding
/// </summary>
/// <param name="device"></param>
/// <param name="texFileNames"></param>
/// <param name="loadType"> 0 = DDS Texture file. 1 = WIC Texture file. </param>
/// <returns></returns>
HRESULT Shaders::PixelShader::InitShaderResources(ID3D11Device* device, const std::vector<std::string>& texFileNames, int loadType)
{
    HRESULT hr = S_OK;

    size_t texCount = texFileNames.size();
    
    for (size_t i = 0; i < texCount; ++i)
    {
        std::wstring widestr = std::wstring(texFileNames[i].begin(), texFileNames[i].end());
        const wchar_t* widecstr = widestr.c_str();

        this->SRVs.push_back(ComPtr<ID3D11ShaderResourceView>());

        if (loadType == 0)
            hr = DirectX::CreateDDSTextureFromFile(device, widecstr, nullptr, this->SRVs[i].GetAddressOf());
        else
            hr = DirectX::CreateWICTextureFromFile(device, widecstr, nullptr, this->SRVs[i].GetAddressOf());
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

/// <summary>
/// Init Shaders to be ready for binding
/// </summary>
/// <param name="device"></param>
/// <param name="texFilename"></param>
/// <param name="loadType"> 0 = DDS Texture file. 1 = WIC Texture file. </param>
/// <returns></returns>
HRESULT Shaders::PixelShader::InitShaderResources(ID3D11Device* device, std::string texFilename, int loadType)
{
    HRESULT hr = S_OK;

    std::wstring widestr = std::wstring(texFilename.begin(), texFilename.end());
    const wchar_t* widecstr = widestr.c_str();
    if (loadType == 0)
        hr = DirectX::CreateDDSTextureFromFile(device, widecstr, nullptr, this->ShaderResourceView.GetAddressOf());
    else
        hr = DirectX::CreateWICTextureFromFile(device, widecstr, nullptr, this->ShaderResourceView.GetAddressOf());

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

HRESULT Shaders::PixelShader::Initialize_ALL(ID3D11Device* device, const char* filename, ID3D11Buffer* constantBuffer, const std::vector<std::string>& texFileNames, int loadType)
{
    HRESULT hr;
    hr = this->Initialize(device, filename, constantBuffer);

    hr = this->InitShaderResources(device, texFileNames, loadType);

    return hr;
}

HRESULT Shaders::PixelShader::Initialize_ALL(ID3D11Device* device, const void* shaderByteCode, ID3D11Buffer* constantBuffer, const std::vector<std::string>& texFileNames, int loadType)
{
    HRESULT hr;
    hr = this->Initialize(device, shaderByteCode, constantBuffer);

    hr = this->InitShaderResources(device, texFileNames, loadType);

    return hr;
}

void Shaders::PixelShader::Bind(ID3D11DeviceContext* deviceContext)
{
    if (this->DeviceContext)
    {
        this->DeviceContext.Reset();
        this->DeviceContext = deviceContext;
    }
    else
        this->DeviceContext = deviceContext;
    if (this->ShaderConstantBuffer)
        this->DeviceContext->PSSetConstantBuffers(0, 1, this->ShaderConstantBuffer.GetAddressOf());
    if (this->Shader)
        this->DeviceContext->PSSetShader(this->Shader.Get(), nullptr, 0);
    this->DeviceContext.Reset();
}

void Shaders::PixelShader::BindShaderResources(ID3D11DeviceContext* deviceContext)
{
    if (this->DeviceContext)
    {
        this->DeviceContext.Reset();
        this->DeviceContext = deviceContext;
    }
    else
        this->DeviceContext = deviceContext;
    if (!this->SRVs.empty())
        this->DeviceContext->PSSetShaderResources(0, this->SRVs.size(), this->SRVs.data()->GetAddressOf());
    if (this->SamplerState)
        this->DeviceContext->PSSetSamplers(0, 1, this->SamplerState.GetAddressOf());
    this->DeviceContext.Reset();
}

void Shaders::PixelShader::BindShaderResources_1(ID3D11DeviceContext* deviceContext)
{
    if (this->DeviceContext)
    {
        this->DeviceContext.Reset();
        this->DeviceContext = deviceContext;
    }
    else
        this->DeviceContext = deviceContext;
    if (this->ShaderResourceView)
        this->DeviceContext->PSSetShaderResources(0, 1, this->ShaderResourceView.GetAddressOf());
    if (this->SamplerState)
        this->DeviceContext->PSSetSamplers(0, 1, this->SamplerState.GetAddressOf());
    this->DeviceContext.Reset();
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

size_t Shaders::PixelShader::GetShaderResourceViews() const
{
    return this->SRVs.size();
}

const ID3D11ShaderResourceView* Shaders::PixelShader::GetShaderResourceView_1() const
{
    return this->ShaderResourceView_1.Get();
}

const ID3D11SamplerState* Shaders::PixelShader::GetSamplerState() const
{
    return this->SamplerState.Get();
}

//
// GEOMETRY SHADER FUNCTIONS =========================================================================
//

HRESULT Shaders::GeometryShader::Initialize(ID3D11Device* device, const char* filename, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    auto blob = load_binary::load_binary_blob(filename);

    // Create the vertex Shader
    hr = device->CreateGeometryShader(blob.data(), blob.size(), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr))
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

HRESULT Shaders::GeometryShader::Initialize(ID3D11Device* device, const void* shaderByteCode, ID3D11Buffer* constantBuffer)
{
    HRESULT hr = S_OK;

    // Create the vertex Shader
    hr = device->CreateGeometryShader(shaderByteCode, sizeof(shaderByteCode), nullptr, this->Shader.GetAddressOf());
    if (FAILED(hr))
        return hr;

    this->ShaderConstantBuffer = constantBuffer;

    //// Create Constant Buffer
    //D3D11_BUFFER_DESC bd;
    //ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = byteWidth;
    //bd.Usage = D3D11_USAGE_DYNAMIC;
    //bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //bd.MiscFlags = 0;
    //hr = device->CreateBuffer(&bd, nullptr, this->ShaderConstantBuffer.GetAddressOf());


    return hr;
}

void Shaders::GeometryShader::Bind(ID3D11DeviceContext* deviceContext)
{
    if (this->DeviceContext)
    {
        this->DeviceContext.Reset();
        this->DeviceContext = deviceContext;
    }
    else
        this->DeviceContext = deviceContext;
    if (this->ShaderConstantBuffer)
        this->DeviceContext->GSSetConstantBuffers(0, 1, this->ShaderConstantBuffer.GetAddressOf());
    if (this->Shader)
        this->DeviceContext->GSSetShader(this->Shader.Get(), nullptr, 0);
    this->DeviceContext.Reset();
}

const ID3D11GeometryShader* Shaders::GeometryShader::GetShader() const
{
    return this->Shader.Get();
}

const ID3D11InputLayout* Shaders::GeometryShader::GetInputLayout() const
{
    return this->InputLayout.Get();
}

const ID3D11Buffer* Shaders::GeometryShader::GetConstantBuffer() const
{
    return this->ShaderConstantBuffer.Get();
}
