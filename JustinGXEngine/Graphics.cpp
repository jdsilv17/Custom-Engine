#include "Graphics.h"



// Model header includes ======================
#include "./Assets/headers/DwarfArmor.h"
#include "./Assets/headers/DwarfAxe.h"
#include "./Assets/headers/DwarfBody.h"
#include "./Assets/headers/DwarfLeather.h"
#include "./Assets/headers/DwarfShirt.h"

#include "./Assets/headers/Planet_1.h"
#include "./Assets/headers/Planet_2.h"
#include "./Assets/headers/Planet_3.h"
#include "./Assets/headers/Moon.h"
#include "./Assets/headers/talon.h"
// ============================================

using namespace DirectX;

bool Graphics::Initialize(HWND hWnd/*, int width, int height*/)
{
    if (!this->InitDirectX(hWnd))
    {
        this->CleanUp();
        return false;
    }

    if (!this->InitShaders())
    {
        this->CleanUp();
        return false;
    }

    if (!this->InitScene())
    {
        this->CleanUp();
        return false;
    }

    return true;
}

void Graphics::RenderFrame()
{
    // rendering here (create function)
    immediateContext->ClearRenderTargetView(RTV, Colors::Black);
    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    // setup pipeline
        // IA (Input Assembler)
        // VS (Vertex Shader)
        // RS (Rasterizer Stage)

    //ID3D10EffectScalarVariable* pActiveViewport;

    D3D11_VIEWPORT vPort_2;
    vPort_2.Width = vPort.Width / 3.0f;
    vPort_2.Height = vPort.Height / 3.0f;
    vPort_2.TopLeftX = vPort_2.TopLeftY = 15.0f;
    vPort_2.MinDepth = 0;
    vPort_2.MaxDepth = 1;
    D3D11_VIEWPORT vPorts[] = { vPort, vPort_2 };
    immediateContext->RSSetViewports(2, vPorts);
    // PS (Pixel Shader)
    // OM (Output Merger)
    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);


    // upload matrices to video card
        // Create and update a constant buffer (move variables from C++ to shaders)
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    HRESULT hr;

    WVP wvp = {};
    XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(XMMatrixIdentity()));
    XMStoreFloat4x4(&wvp.sView, XMMatrixTranspose(cam.GetViewMatrix()));
    XMStoreFloat4x4(&wvp.sProjection, XMMatrixTranspose(cam.GetProjectionMatrix()));

    XMMATRIX temp_LtRotY = XMMatrixRotationY(-0.01f);
    //pntLight.SetPosition(XMVector3Transform(pntLight.GetPositionVector(), temp_LtRotY));
    //sptLight.SetPosition(XMVector3Transform(sptLight.GetPositionVector(), temp_LtRotY));
    XMStoreFloat4(&wvp.LightPos[0], dirLight.GetPositionVector());
    XMStoreFloat4(&wvp.LightPos[1], pntLight.GetPositionVector());
    XMStoreFloat4(&wvp.LightPos[2], sptLight.GetPositionVector());

    XMMATRIX temp_LtRotZ = XMMatrixRotationZ(-0.01f);
    //dirLight.SetDirection(XMVector3Transform(dirLight.GetDirectionVector(), temp_LtRotZ));
    //sptLight.SetDirection(XMVector3Transform(sptLight.GetConeDirectionVector(), temp_LtRotY));
    XMStoreFloat4(&wvp.LightDir[0], dirLight.GetDirectionVector());
    XMStoreFloat4(&wvp.LightDir[1], sptLight.GetConeDirectionVector());

    XMStoreFloat4(&wvp.LightColor[0], dirLight.GetLightColorVector());
    XMStoreFloat4(&wvp.LightColor[1], pntLight.GetLightColorVector());
    XMStoreFloat4(&wvp.LightColor[2], sptLight.GetLightColorVector());
    XMStoreFloat4(&wvp.CamPos, cam.GetPositionVector());
    //wvp.totalTime.x = (float)gTimer.deltaTime / 1000.0f;

    

    auto jointSize = (run_anim.IsPlaying()) ? run_anim.TweenJoints.size() : run_anim.GetCurrentKeyframe()->joints.size();
    for (size_t i = 0; i < jointSize; ++i)
    {
        DirectX::XMMATRIX mat;
        if (run_anim.IsPlaying())
            mat = XMLoadFloat4x4(&run_anim.TweenJoints[i].global_xform);
        else
            mat = XMLoadFloat4x4(&run_anim.GetCurrentKeyframe()->joints[i].global_xform);
        DirectX::XMMATRIX invMat = XMLoadFloat4x4(&run_anim.Frames[0].joints[i].inv_xform);
        invMat = XMMatrixTranspose(invMat * mat);
        XMStoreFloat4x4(&wvp.SkinMat[i], invMat);
    }

    //======================================================================================================================

    // Draw Point to Quad ==================================
    if (DrawQuad)
    {
        gs_VS.Bind(immediateContext);
        solid_PS.Bind(immediateContext);
        pntToQuad_GS.Bind(immediateContext); // shader signatures are incompatible
        //cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
        XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(XMMatrixIdentity()));
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0);

        point.Draw();
        immediateContext->GSSetShader(nullptr, nullptr, 0);
    }


    // Draw Skybox =====================================
    if (true)
    {
        immediateContext->RSSetState(RSCullNone); // turn back face culling off
        immediateContext->OMSetDepthStencilState(DSLessEqual, 0); // draw skybox everywhere that is not drawn on
        skybox_VS.Bind(immediateContext);
        skybox_PS.Bind(immediateContext);
        skybox_PS.BindShaderResources_1(immediateContext);
        XMMATRIX m_skybox = XMMatrixTranspose(XMMatrixTranslationFromVector(cam.GetPositionVector()));
        XMStoreFloat4x4(&wvp.sWorld, m_skybox);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0);

        skybox.Mesh.Draw();
        immediateContext->RSSetState(nullptr);
        immediateContext->OMSetDepthStencilState(nullptr, 0);
    }

    // Draw fbx mesh
    anim_VS.Bind(immediateContext);
    anim_PS.Bind(immediateContext);
    anim_PS.BindShaderResources(immediateContext);
    //cb.mWorld = XMMatrixTranspose(BattleMage.GetWorldMatrix());
    XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(BattleMage.GetWorldMatrix()));
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0);
    BattleMage.Draw();

    // Draw Point Light
    //solid_PS.Bind(immediateContext);
    //cb.mWorld = XMMatrixTranspose(XMMatrixScaling(0.05f, 0.05f, 0.05f) * pntLight.GetWorldMatrix());
    XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(XMMatrixScaling(0.05f, 0.05f, 0.05f) * pntLight.GetWorldMatrix()));
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0);
    BattleMage.Draw();


    default_VS.Bind(immediateContext);
    viewportTEST_GS.Bind(immediateContext); // used to have multiple viewports
    solid_PS.Bind(immediateContext);
    // Draw Grid ========================================
    immediateContext->RSSetState(RSAALLines);
    if (!DrawGrid)
    {
        //cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
        XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(XMMatrixIdentity()));
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
        grid.Draw();
    }

    // Draw Debug_renderer ========================================
    immediateContext->OMSetDepthStencilState(DSNoDepth, 0);
    Mesh<VERTEX> debug_lines = Mesh<VERTEX>(myDevice, immediateContext,
        end::debug_renderer::get_line_verts(),
        (int)end::debug_renderer::get_line_vert_capacity(),
        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    //cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
    XMStoreFloat4x4(&wvp.sWorld, XMMatrixTranspose(XMMatrixIdentity()));
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
    debug_lines.Draw();

    end::debug_renderer::clear_lines();

    immediateContext->GSSetShader(nullptr, nullptr, NULL);
    immediateContext->RSSetState(nullptr);
    immediateContext->OMSetDepthStencilState(nullptr, 0);

    bool vysnc = true;
    swapChain->Present(vysnc, 0);
}

/// <summary>
/// Cleans up and releases any objects that were created
/// </summary>
void Graphics::CleanUp()
{
    //size_t size = terrain_triangles.size();
    //for (size_t i = 0; i < size; ++i)
    //{
    //    delete[] terrain_triangles[i];
    //}

    if (immediateContext) immediateContext->ClearState();

    if (RTV) RTV->Release();
    if (zBuffer) zBuffer->Release();
    if (zBufferView) zBufferView->Release();
    if (RSAALLines) RSAALLines->Release();
    if (RSCullNone) RSCullNone->Release();
    if (DSLessEqual) DSLessEqual->Release();
    if (DSNoDepth) DSNoDepth->Release();
    if (blendState) blendState->Release();
    if (swapChain) swapChain->Release();
    if (immediateContext) immediateContext->Release();
    if (myDevice) myDevice->Release();
}

bool Graphics::InitDirectX(HWND hWnd)
{
    HRESULT hr = S_OK;

    // Temporarily here
    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
    aspectRatio = width / float(height);

    // attach d3d11 to window
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    //D3D_FEATURE_LEVEL featureLevel[] =
    //{
    //    D3D_FEATURE_LEVEL_11_0,
    //    D3D_FEATURE_LEVEL_11_1,
    //    D3D_FEATURE_LEVEL_10_0,
    //    D3D_FEATURE_LEVEL_10_1
    //};

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // Create Swap Chain and Device
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;



    hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, NULL, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &sd,
        &swapChain, &myDevice, 0, &immediateContext);
    if (FAILED(hr))
        return false;

    //#ifdef _DEBUG
    //    hr = myDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
    //    //hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    //#endif

    // Create Render Target View
    ID3D11Resource* backbuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(backbuffer), reinterpret_cast<void**>(&backbuffer));
    if (FAILED(hr))
        return false;

    hr = myDevice->CreateRenderTargetView(backbuffer, NULL, &RTV);
    backbuffer->Release();
    if (FAILED(hr))
        return false;


    // create zbuffer & view
    D3D11_TEXTURE2D_DESC zd;
    ZeroMemory(&zd, sizeof(zd));
    zd.Width = width;
    zd.Height = height;
    zd.MipLevels = 1;
    zd.ArraySize = 1;
    zd.Format = DXGI_FORMAT_D32_FLOAT;
    zd.SampleDesc.Count = 1;
    zd.Usage = D3D11_USAGE_DEFAULT;
    zd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = myDevice->CreateTexture2D(&zd, nullptr, &zBuffer);
    if (FAILED(hr))
        return false;

    // Create the depth stencil view
    //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    //descDSV.Format = descDepth.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.Texture2D.MipSlice = 0;
    hr = myDevice->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);
    if (FAILED(hr))
        return false;

    // Setup viewport
    vPort.Width = static_cast<FLOAT>(width);
    vPort.Height = static_cast<FLOAT>(height);
    vPort.TopLeftX = vPort.TopLeftY = 0;
    vPort.MinDepth = 0;
    vPort.MaxDepth = 1;

    //#ifdef _DEBUG
    //    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    //#endif

    return true;
}

bool Graphics::InitShaders()
{
    HRESULT hr = S_OK;

    // Create Constant Buffer
    ComPtr<ID3D11Buffer> constantBuffer = nullptr;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(WVP);
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = myDevice->CreateBuffer(&bd, nullptr, constantBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    // write, compile & load our shaders
    hr = advanced_VS.Initialize(myDevice, "./Shaders/MeshVertexShader.cso", objLayoutDesc, ARRAYSIZE(objLayoutDesc), constantBuffer.Get());
    //advanced_VS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;
    hr = default_VS.Initialize(myDevice, "./Shaders/VertexShader.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), constantBuffer.Get());
    //default_VS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;
    hr = anim_VS.Initialize(myDevice, "./Shaders/Anim_VS.cso", animInputLayoutDesc, ARRAYSIZE(animInputLayoutDesc), constantBuffer.Get());
    //anim_VS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;
    hr = skybox_VS.Initialize(myDevice, "./Shaders/SkyBox_VS.cso", cubeLayoutDesc, ARRAYSIZE(cubeLayoutDesc), constantBuffer.Get());
    //skybox_VS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;
    hr = gs_VS.Initialize(myDevice, "./Shaders/Geo_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), constantBuffer.Get());
    //gs_VS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;
    //hr = HUD_VS.Initialize(myDevice, "./HUD_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));
    //HUD_VS.ShaderConstantBuffer = constantBuffer;
    //if (FAILED(hr)) return false;
    //hr = Smoke_VS.Initialize(myDevice, "./HUD_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));
    //Smoke_VS.ShaderConstantBuffer = constantBuffer;
    //if (FAILED(hr)) return false;

    hr = solid_PS.Initialize(myDevice, "./Shaders/PS_Solid.cso", constantBuffer.Get());
    //solid_PS.ShaderConstantBuffer = constantBuffer;
    if (FAILED(hr)) return false;

    // read FBX materials
    using Path = std::array<char, 260>;
    std::vector<material_t> materials;
    std::vector<Path> paths;
    load_binary::Load_FBXMat_blob("./Assets/Textures/BattleMageRun.mats", materials, paths);
    std::vector<std::string> in_paths;
    in_paths.resize(paths.size());
    for (size_t i = 0; i < paths.size(); ++i)
    {
        in_paths[i] = ".\\Assets\\Textures\\";
        in_paths[i].append(paths[i].data());
    }
    hr = anim_PS.Initialize_ALL(myDevice, "./Shaders/Anim_PS.cso", constantBuffer.Get(), in_paths, 1);
    if (FAILED(hr)) return false;
    //hr = anim_PS.InitShaderResources(myDevice, in_paths, 1);
    //if (FAILED(hr)) return false;
    //anim_PS.ShaderConstantBuffer = anim_VS.ShaderConstantBuffer;

    // Note: change Initialize_ALL to be more versatile
    hr = skybox_PS.Initialize(myDevice, "./Shaders/SkyBox_PS.cso", constantBuffer.Get());
    if (FAILED(hr)) return false;
    hr = skybox_PS.InitShaderResources(myDevice, "./Assets/Textures/SunsetSkybox.dds");
    if (FAILED(hr)) return false;
    //skybox_PS.ShaderConstantBuffer = skybox_VS.ShaderConstantBuffer;

    hr = pntToQuad_GS.Initialize(myDevice, "./Shaders/PointToQuad_GS.cso", constantBuffer.Get());
    if (FAILED(hr)) return false;
    //pntToQuad_GS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;

    hr = viewportTEST_GS.Initialize(myDevice, "./Shaders/ViewportTEST_GS.cso", constantBuffer.Get());
    if (FAILED(hr)) return false;


#pragma region Dwarf Shaders
    //std::string textures[2] = { "./Assets/Textures/Dwarf/BodyTexture.dds", "./Assets/Textures/Dwarf/BodySpec.dds" };
    //hr = DwarfBody_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    //hr = DwarfBody_PS.InitShaderResources(myDevice, textures);
    //DwarfBody_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    //textures->clear(); 
    //textures[0] = "./Assets/Textures/Dwarf/ShirtTexture.dds";
    //textures[1] = "./Assets/Textures/Dwarf/ShirtSpec.dds";
    //hr = DwarfShirt_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    //hr = DwarfShirt_PS.InitShaderResources(myDevice, textures);
    //DwarfShirt_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    //textures->clear();
    //textures[0] = "./Assets/Textures/Dwarf/LeatherTexture.dds";
    //textures[1] = "./Assets/Textures/Dwarf/LeatherSpec.dds";
    //hr = DwarfLeather_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    //hr = DwarfLeather_PS.InitShaderResources(myDevice, textures);
    //DwarfLeather_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    //textures->clear();
    //textures[0] = "./Assets/Textures/Dwarf/ArmorTexture.dds";
    //textures[1] =  "./Assets/Textures/Dwarf/ArmorSpec.dds";
    //hr = DwarfArmor_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    //hr = DwarfArmor_PS.InitShaderResources(myDevice, textures);
    //DwarfArmor_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    //hr = DwarfAxe_PS.Initialize(myDevice, "./SkyBoxReflection_PS.cso", sizeof(WVP));
    //hr = DwarfAxe_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LostValley.dds");
    //DwarfAxe_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    //hr = HUD_PS.Initialize(myDevice, "./HUD_PS.cso", sizeof(WVP));
    //hr = HUD_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LoadingText.dds");
    //HUD_PS.ShaderConstantBuffer = HUD_VS.ShaderConstantBuffer;
    //hr = Smoke_PS.Initialize(myDevice, "./Smoke_PS.cso", sizeof(WVP));
    //hr = Smoke_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LoadingSmoke.dds");
    //Smoke_PS.ShaderConstantBuffer = Smoke_VS.ShaderConstantBuffer;
#pragma endregion

#pragma region SpaceScene Shaders
//hr = planet1_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
//hr = planet1_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet_Diffuse.dds");
//planet1_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

//hr = planet2_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
//hr = planet2_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet2_Diffuse.dds");
//planet2_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

//hr = planet3_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
//hr = planet3_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet4_Diffuse.dds");
//planet3_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

//hr = moon_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
//hr = moon_PS.InitShaderResources(myDevice, "./Assets/Textures/moon_Diffuse.dds");
//moon_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

//hr = talon_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
//hr = talon_PS.InitShaderResources(myDevice, "./Assets/Textures/defender.dds");
//talon_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
#pragma endregion


    // TEXTURING ===============================================================================================

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.CullMode = D3D11_CULL_NONE;
    rd.FillMode = D3D11_FILL_SOLID;
    hr = myDevice->CreateRasterizerState(&rd, &RSCullNone);
    if (FAILED(hr)) return false;
    rd.AntialiasedLineEnable = true;
    rd.CullMode = D3D11_CULL_BACK;
    rd.FillMode = D3D11_FILL_SOLID;
    hr = myDevice->CreateRasterizerState(&rd, &RSAALLines);
    if (FAILED(hr)) return false;

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = myDevice->CreateDepthStencilState(&depthStencilDesc, &DSLessEqual);
    if (FAILED(hr)) return false;
    depthStencilDesc.DepthEnable = false;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = myDevice->CreateDepthStencilState(&depthStencilDesc, &DSNoDepth);
    if (FAILED(hr)) return false;

    // Blending =====================================================================
    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));
    rtbd.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = true;
    blendDesc.RenderTarget[0] = rtbd;
    hr = myDevice->CreateBlendState(&blendDesc, &blendState);
    if (FAILED(hr)) return false;

    //#ifdef _DEBUG
    //    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    //#endif

    return true;
}

bool Graphics::InitScene()
{

#pragma region Dwarf Meshes
    //std::vector<_OBJ_VERT_> verts;
    //std::vector<int> indices;
    //// CREATE DWARFBODY
    //numOfVerts = ARRAYSIZE(DwarfBody_data);
    //for (size_t i = 0; i < numOfVerts; ++i)
    //    verts.push_back(DwarfBody_data[i]);
    //numOfElements = ARRAYSIZE(DwarfBody_indicies);
    //for (size_t i = 0; i < numOfElements; ++i)
    //    indices.push_back(DwarfBody_indicies[i]);
    //DwarfBody = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //DwarfBody.SetPosition(-1.0f, 0.0f, 0.0f);
    //DwarfBody.SetRotation(0.0f, XM_PI, 0.0f);

    //verts.clear(); verts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();
    //// CREATE DWARFSHIRT
    //numOfVerts = ARRAYSIZE(DwarfShirt_data);
    //for (size_t i = 0; i < numOfVerts; ++i)
    //    verts.push_back(DwarfShirt_data[i]);
    //numOfElements = ARRAYSIZE(DwarfShirt_indicies);
    //for (size_t i = 0; i < numOfElements; ++i)
    //    indices.push_back(DwarfShirt_indicies[i]);
    //DwarfShirt = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //verts.clear(); verts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();
    //// CREATE DWARFLEATHER
    //numOfVerts = ARRAYSIZE(DwarfLeather_data);
    //for (size_t i = 0; i < numOfVerts; ++i)
    //    verts.push_back(DwarfLeather_data[i]);
    //numOfElements = ARRAYSIZE(DwarfLeather_indicies);
    //for (size_t i = 0; i < numOfElements; ++i)
    //    indices.push_back(DwarfLeather_indicies[i]);
    //DwarfLeather = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //verts.clear(); verts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();
    //// CREATE DwarfArmor
    //numOfVerts = ARRAYSIZE(DwarfArmor_data);
    //for (size_t i = 0; i < numOfVerts; ++i)
    //    verts.push_back(DwarfArmor_data[i]);
    //numOfElements = ARRAYSIZE(DwarfArmor_indicies);
    //for (size_t i = 0; i < numOfElements; ++i)
    //    indices.push_back(DwarfArmor_indicies[i]);
    //DwarfArmor = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //verts.clear(); verts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();

    //// CREATE DwarfAxe
    //numOfVerts = ARRAYSIZE(DwarfAxe_data);
    //for (size_t i = 0; i < numOfVerts; ++i)
    //    verts.push_back(DwarfAxe_data[i]);
    //numOfElements = ARRAYSIZE(DwarfAxe_indicies);
    //for (size_t i = 0; i < numOfElements; ++i)
    //    indices.push_back(DwarfAxe_indicies[i]);
    //DwarfAxe = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //DwarfAxe.SetPosition(-1.0f, 0.0f, 0.0f);
    //DwarfAxe.SetRotation(0.0f, 180.0f * (XM_PI / 180.0f), 0.0f);

    //verts.clear(); verts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();

    //// CREATE HUD
    //std::vector<VERTEX> HUDverts;
    //HUDverts =
    //{
    //    VERTEX({-1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}),
    //    VERTEX({1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}),
    //    VERTEX({-1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}),
    //    VERTEX({1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f})
    //};
    //indices =
    //{
    //    0,1,2,
    //    2,1,3
    //};
    //HUD = Mesh<VERTEX>(myDevice, immediateContext, HUDverts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //HUDverts.clear(); HUDverts.shrink_to_fit();
    //indices.clear(); indices.shrink_to_fit();
#pragma endregion

#pragma region SpaceScene Meshes
//// PLANET_1 =================================
//std::vector<_OBJ_VERT_> planetVerts;
//numOfVerts = ARRAYSIZE(Planet_1_data);
//for (size_t i = 0; i < numOfVerts; ++i)
//    planetVerts.push_back(Planet_1_data[i]);
//std::vector<int> planetIndices;
//numOfElements = ARRAYSIZE(Planet_1_indicies);
//for (size_t i = 0; i < numOfElements; ++i)
//    planetIndices.push_back(Planet_1_indicies[i]);
//planet_1 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//// PLANET_2 =================================
//planetVerts.clear();
//planetIndices.clear();
//numOfVerts = ARRAYSIZE(Planet_2_data);
//for (size_t i = 0; i < numOfVerts; ++i)
//    planetVerts.push_back(Planet_2_data[i]);
//numOfElements = ARRAYSIZE(Planet_2_indicies);
//for (size_t i = 0; i < numOfElements; ++i)
//    planetIndices.push_back(Planet_2_indicies[i]);
//planet_2 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//// PLANET_3 =================================
//planetVerts.clear();
//planetIndices.clear();
//numOfVerts = ARRAYSIZE(Planet_3_data);
//for (size_t i = 0; i < numOfVerts; ++i)
//    planetVerts.push_back(Planet_3_data[i]);
//numOfElements = ARRAYSIZE(Planet_3_indicies);
//for (size_t i = 0; i < numOfElements; ++i)
//    planetIndices.push_back(Planet_3_indicies[i]);
//planet_3 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//// MOON ====================================
//planetVerts.clear();
//planetIndices.clear();
//numOfVerts = ARRAYSIZE(Moon_data);
//for (size_t i = 0; i < numOfVerts; ++i)
//    planetVerts.push_back(Moon_data[i]);
//numOfElements = ARRAYSIZE(Moon_indicies);
//for (size_t i = 0; i < numOfElements; ++i)
//    planetIndices.push_back(Moon_indicies[i]);
//moon = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//// TALON ===================================
//std::vector<_OBJ_VERT_> talonVerts;
//numOfVerts = ARRAYSIZE(talon_data);
//for (size_t i = 0; i < numOfVerts; ++i)
//    talonVerts.push_back(talon_data[i]);
//std::vector<int> talonIndices;
//numOfElements = ARRAYSIZE(talon_indicies);
//for (size_t i = 0; i < numOfElements; ++i)
//    talonIndices.push_back(talon_indicies[i]);
//talon = Mesh<_OBJ_VERT_>(myDevice, immediateContext, talonVerts, talonIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion

    // point to quad
    std::vector<VERTEX> pnt_Vert;
    pnt_Vert.push_back(VERTEX({ 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }));
    point = Mesh<VERTEX>(myDevice, immediateContext, pnt_Vert, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    // Skybox
    skybox.Mesh = Mesh<VERTEX_BASIC>(myDevice, immediateContext, skybox._vertexList, skybox._indicesList, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    // Grid
    auto gridVerts = MakeGrid(20.0f, 24);
    grid = Mesh<VERTEX>(myDevice, immediateContext, gridVerts.data(), (int)gridVerts.size(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    
    // Initialize sorted pool
    for (uint16_t i = 0; i < sortPool.capacity(); ++i)
    {
        sortPool[i].Velocity = { RAND_FLT(-3.0f, 3.0f), 15.0f, RAND_FLT(-3.0f, 3.0f), 0.0f };
        sortPool[i].Lifetime = 3.0f;
    }

    // initialize particle emitters
    emitters[0].SetSpawnPosition(-9.0f, 0.0f, 9.0f);
    emitters[0].Spawn_Color = { 0.0f, 1.0f, 0.0f, 1.0f };
    emitters[1].SetSpawnPosition(-9.0f, 0.0f, -9.0f);
    emitters[1].Spawn_Color = { 1.0f, 0.0f, 1.0f, 1.0f };
    emitters[2].SetSpawnPosition(9.0f, 0.0f, 9.0f);
    emitters[2].Spawn_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
    emitters[3].SetSpawnPosition(9.0f, 0.0f, -9.0f);
    emitters[3].Spawn_Color = { 1.0f, 1.0f, 1.0f, 1.0f };

    // initialize gizmos
    Object gizmo_1;
    Object gizmo_2;
    Object gizmo_3;
    gizmo_1.SetPosition(0.0f, 1.0f, 0.0f);
    gizmo_2.SetPosition(-3.0f, 3.0f, -2.0f);
    gizmo_3.SetPosition(3.0f, 3.0f, 2.0f);
    Gizmos.push_back(gizmo_1);
    Gizmos.push_back(gizmo_2);
    Gizmos.push_back(gizmo_3);

    // initialize AABBs
    for (size_t i = 0; i < ARRAYSIZE(aabbs); ++i)
    {
        aabbs[i].center = { RAND_FLT(-10.0f, 10.0f), 0.0f, RAND_FLT(-10.0f, 10.0f) };
        float w = RAND_FLT(1.0f, 1.0f);
        //XMStoreFloat3(&aabbs[i].center, AABB[i].GetPositionVector());
        float x = RAND_FLT(0.5f, 2.0f);
        float y = RAND_FLT(0.2f, 2.0f);
        float z = RAND_FLT(0.2f, 2.0f);
        aabbs[i].extents = { x, y, z };
    }

    //// Load terrain binary
    //std::vector<XMFLOAT3> terrain_pos = load_binary::load_terrain_blob("./Assets/headers/terrain.bin");
    //terrain_pos.shrink_to_fit();
    //terrain_verts.resize(terrain_pos.size());
    //size_t vert_count = terrain_verts.size();
    //for (size_t i = 0; i < vert_count; ++i)
    //{
    //    terrain_verts[i] = VERTEX({ terrain_pos[i].x, terrain_pos[i].y, terrain_pos[i].z, 1.0f });
    //}
    //// Create an array of triangles which is an array of 3 vertices
    //size_t numTriangles = vert_count / 3;
    //terrain_tri_indices.resize(numTriangles);
    //terrain_triangles.resize(numTriangles);
    //terrain_centroids.resize(numTriangles);
    //size_t pos = 0;
    //for (size_t i = 0; i < numTriangles; ++i)
    //{
    //    terrain_tri_indices[i] = i;
    //    terrain_triangles[i] = new VERTEX[3];

    //    for (size_t j = 0; j < 3; ++j)
    //    {
    //        terrain_triangles[i][j] = terrain_verts[pos];
    //        ++pos;
    //    }

    //    XMVECTOR a = XMLoadFloat4(&terrain_triangles[i][0].pos);
    //    XMVECTOR b = XMLoadFloat4(&terrain_triangles[i][1].pos);
    //    XMVECTOR c = XMLoadFloat4(&terrain_triangles[i][2].pos);
    //    XMVECTOR avg = (a + b + c) / 3.0f;
    //    XMStoreFloat4(&terrain_centroids[i], avg);
    //}

    // Load BattleMage.fbx
    std::vector<int> indexList;
    std::vector<VERTEX_ANIM> vertices;
    load_binary::Load_FBXMesh_blob("./Assets/headers/Run.mesh", indexList, vertices);
    BattleMage = Mesh<VERTEX_ANIM>(myDevice, immediateContext, vertices.data(), vertices.size(),
        indexList.data(), indexList.size(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    double _duration;
    int _frameCount;
    load_binary::Load_FBXAnim_blob("./Assets/Animations/Run.anim",
        run_anim.Frames, _duration, _frameCount);
    run_anim.SetAnimationLength(_duration);
    run_anim.SetFrameCount(_frameCount);

    for (size_t frame = 0; frame < _frameCount; ++frame)
    {
        size_t jointCount = run_anim.Frames[frame].joints.size();
        for (size_t j = 0; j < jointCount; ++j)
        {
            run_anim.Frames[frame].joints[j].jointObject.SetWorld(run_anim.GetKeyframes()[frame].joints[j].global_xform);
        }
    }

    // initialize camera
    cam.SetPosition(0.0f, 5.0f, 5.0f);
    cam.SetRotation(0.0f, XM_PI, 0.0f);
    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f); // add functionality to adjust camera settings

    // initialize Directional Light
    dirLight.SetPosition(-20.0f, 20.0f, 0.0f);
    dirLight.SetDirection(-1.0f, 0.0f, -0.2f);
    dirLight.SetLightColor(0.75f, 0.75f, 0.75f, 1.0f);
    dirLight.SetAmbientTerm(0.3f);
    // initialize Point Light
    pntLight.SetPosition(0.0f, 4.0f, 1.0f);
    pntLight.SetLightColor(1.0f, 1.0f, 1.0f, 1.0f);
    pntLight.SetAmbientTerm(0.9f);
    pntLight.SetPointRadius(35.0f);
    // initialize Spot Light
    sptLight.SetPosition(0.0f, 12.0f, 0.0f);
    sptLight.SetDirection(0.0f, -1.0f, 0.0f);
    sptLight.SetLightColor(0.0f, 1.0f, 0.0f, 1.0f);
    sptLight.SetAmbientTerm(0.1f);
    sptLight.SetOuterInnerConeRatios(0.5f, 0.8f);

    return true;
}

#pragma region SCENE_FUNCS
//void DrawSpaceScene()
//{
//    // rendering here (create function)
//    immediateContext->ClearRenderTargetView(RTV, Colors::DarkBlue);
//    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//    // setup pipeline
//        // IA (Input Assembler)
//        // VS (Vertex Shader)
//        // RS (Rasterizer Stage)
//    immediateContext->RSSetViewports(1, &vPort);
//    // PS (Pixel Shader)
//    // OM (Output Merger)
//    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);
//
//
//    // upload matrices to video card
//        // Create and update a constant buffer (move variables from C++ to shaders)
//    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
//    HRESULT hr;
//
//    //static float rot = 0; rot += 0.005f;
//    ConstantBuffer cb = {};
//
//    cb.mView = XMMatrixTranspose(cam.GetViewMatrix());
//
//    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
//    cb.mProjection = XMMatrixTranspose(cam.GetProjectionMatrix());
//
//    WVP wvp = {};
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    XMStoreFloat4x4(&wvp.sView, cb.mView);
//    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);
//
//    XMMATRIX temp_LtRotY = XMMatrixRotationY(-0.01f);
//    //pntLight.SetPosition(XMVector3Transform(pntLight.GetPositionVector(), temp_LtRotY));
//    //sptLight.SetPosition(XMVector3Transform(sptLight.GetPositionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightPos[0], dirLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[1], pntLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[2], sptLight.GetPositionVector());
//
//    XMMATRIX temp_LtRotZ = XMMatrixRotationZ(-0.01f);
//    //dirLight.SetDirection(XMVector3Transform(dirLight.GetDirectionVector(), temp_LtRotZ));
//    //sptLight.SetDirection(XMVector3Transform(sptLight.GetConeDirectionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightDir[0], dirLight.GetDirectionVector());
//    XMStoreFloat4(&wvp.LightDir[1], sptLight.GetConeDirectionVector());
//
//    XMStoreFloat4(&wvp.LightColor[0], dirLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[1], pntLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[2], sptLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.CamPos, cam.GetPositionVector());
//    //wvp.totalTime.x = (float)gTimer.deltaTime / 1000.0f;
//
//    //======================================================================================================================
//
//    // Draw Planet_1 ===========================================
//    advanced_VS.Bind(immediateContext);
//    planet1_PS.Bind(immediateContext);
//    planet1_PS.BindShaderResources_1(immediateContext);
//    XMMATRIX mtranlsation = XMMatrixTranslation(0.0f, 0.0f, 60.0f);
//    XMMATRIX mscale = XMMatrixScaling(0.023f, 0.023f, 0.023f);
//    XMMATRIX mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 0.5f);
//    XMMATRIX Planet1m = mscale * mtranlsation * mrotationY;
//    cb.mWorld = XMMatrixTranspose(Planet1m);
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    planet_1.Draw();
//
//    // Draw Planet_2 ===========================================
//    planet2_PS.Bind(immediateContext);
//    planet2_PS.BindShaderResources_1(immediateContext);
//
//    mtranlsation = XMMatrixTranslation(0.0f, 0.0f, -220.0f);
//    mscale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
//    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 0.35f);
//    XMMATRIX Planet2m = mscale * mtranlsation * mrotationY;
//    cb.mWorld = XMMatrixTranspose(Planet2m);
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    planet_2.Draw();
//
//    // Draw Planet_3 ===========================================
//    planet3_PS.Bind(immediateContext);
//    planet3_PS.BindShaderResources_1(immediateContext);
//
//    mtranlsation = XMMatrixTranslation(-12.0f, 0.0f, 0.0f);
//    mscale = XMMatrixScaling(0.010f, 0.010f, 0.010f);
//    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 1.1f);
//    XMMATRIX Planet3m = mscale * mtranlsation * mrotationY;
//    cb.mWorld = XMMatrixTranspose(Planet3m);
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    planet_3.Draw();
//
//    // Draw Moon ===========================================
//    moon_PS.Bind(immediateContext);
//    moon_PS.BindShaderResources_1(immediateContext);
//
//    mtranlsation = XMMatrixTranslation(800.0f, 0.0f, 0.0f);
//    mscale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
//    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * XM_PI);
//    XMMATRIX Moonm = mscale * mtranlsation * mrotationY;
//    cb.mWorld = XMMatrixTranspose(Moonm * Planet1m);
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    moon.Draw();
//
//    // Draw Ship =======================================
//    talon_PS.Bind(immediateContext);
//    talon_PS.BindShaderResources_1(immediateContext);
//    cb.mWorld = XMMatrixTranspose(XMMatrixTranslation(0.0f, -0.5f, 2.0f) * cam.GetWorldMatrix());
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    talon.Draw();
//
//    // Draw Point to Quad ==================================
//    if (DrawQuad)
//    {
//        solid_PS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;
//        gs_VS.Bind(immediateContext);
//        solid_PS.Bind(immediateContext);
//        pntToQuad_GS.Bind(immediateContext);
//        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0);
//
//        point.Draw();
//        immediateContext->GSSetShader(nullptr, nullptr, 0);
//    }
//
//    // Draw Skybox =====================================
//    immediateContext->RSSetState(RSCullNone); // turn back face culling off
//    immediateContext->OMSetDepthStencilState(DSLessEqual, 0); // draw skybox everywhere that is not drawn on
//    skybox_VS.Bind(immediateContext);
//    skybox_PS.Bind(immediateContext);
//    skybox_PS.BindShaderResources_1(immediateContext);
//    cb.mWorld = XMMatrixTranspose(XMMatrixTranslationFromVector(cam.GetPositionVector()));
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0);
//
//    skybox.Mesh.Draw();
//    immediateContext->RSSetState(nullptr);
//    immediateContext->OMSetDepthStencilState(nullptr, 0);
//
//    // Draw Grid ========================================
//
//    if (DrawGrid)
//    {
//        //if (solid_PS.ShaderConstantBuffer != default_VS.ShaderConstantBuffer)
//        //{
//        //    solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer;
//        //}
//
//        default_VS.Bind(immediateContext);
//        solid_PS.Bind(immediateContext);
//        std::vector<VERTEX> lines = MakeGrid(20.0f, 25);
//        //std::vector<VERTEX> lines = MakeColorGrid(20.0f, 25, (float)gTimer.deltaTime);
//        //grid = Mesh<VERTEX>(myDevice, immediateContext, lines, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
//
//        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
//        grid.Draw();
//    }
//
//    // change 1 to 0 vsync
//    bool vysnc = true;
//    swapChain->Present(vysnc, 0);
//}
//
//void DrawDwarfScene()
//{
//    // rendering here (create function)
//    immediateContext->ClearRenderTargetView(RTV, Colors::Black);
//    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//    // setup pipeline
//        // IA (Input Assembler)
//        // VS (Vertex Shader)
//        // RS (Rasterizer Stage)
//    immediateContext->RSSetViewports(1, &vPort);
//    // PS (Pixel Shader)
//    // OM (Output Merger)
//    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);
//    immediateContext->OMSetBlendState(blendState, 0, 0xffffffff);
//
//    // upload matrices to video card
//        // Create and update a constant buffer (move variables from C++ to shaders)
//    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
//    HRESULT hr;
//
//    XMMATRIX temp2 = XMMatrixTranslation(1.5f, 0, 0);
//    ConstantBuffer cb = {};
//
//    cb.mView = XMMatrixTranspose(cam.GetViewMatrix());
//
//    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
//    cb.mProjection = XMMatrixTranspose(cam.GetProjectionMatrix());
//
//    WVP wvp = {};
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    XMStoreFloat4x4(&wvp.sView, cb.mView);
//    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);
//
//    XMMATRIX temp_LtRotY = XMMatrixRotationY(-0.01f);
//    //pntLight.SetPosition(XMVector3Transform(pntLight.GetPositionVector(), temp_LtRotY));
//    //sptLight.SetPosition(XMVector3Transform(sptLight.GetPositionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightPos[0], dirLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[1], pntLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[2], sptLight.GetPositionVector());
//
//    XMMATRIX temp_LtRotZ = XMMatrixRotationZ(-0.01f);
//    //dirLight.SetDirection(XMVector3Transform(dirLight.GetDirectionVector(), temp_LtRotZ));
//    //sptLight.SetDirection(XMVector3Transform(sptLight.GetConeDirectionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightDir[0], dirLight.GetDirectionVector());
//    XMStoreFloat4(&wvp.LightDir[1], sptLight.GetConeDirectionVector());
//
//    XMStoreFloat4(&wvp.LightColor[0], dirLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[1], pntLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[2], sptLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.CamPos, cam.GetPositionVector());
//    wvp.totalTime.x = GetTickCount64() / 1000.f;
//
//    //======================================================================================================================
//    static float time = 0.0f;
//    static ULONGLONG timeStart = 0;
//    ULONGLONG timeCur = GetTickCount64();
//    if (timeStart == 0)
//        timeStart = timeCur;
//    time = (timeCur - timeStart) / 1000.0f;
//
//    // Draw DwarfBody ===========================================
//    advanced_VS.Bind(immediateContext);
//    DwarfBody_PS.Bind_ALL(immediateContext);
//    XMMATRIX temp;
//    XMMATRIX mtranlsation = XMMatrixTranslation(0.0f, 0.0f, 60.0f);
//    XMMATRIX mscale = XMMatrixScaling(0.023f, 0.023f, 0.023f);
//    XMMATRIX mrotation = XMMatrixRotationY((6.0f * (XM_PI / 180.0f)) * (float)gTimer.deltaTime);
//    cb.mWorld = XMMatrixTranspose(mrotation * DwarfBody.GetWorldMatrix());
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    DwarfBody.Draw();
//
//    // Draw DwarfShirt ===========================================
//    DwarfShirt_PS.Bind_ALL(immediateContext);
//    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());
//
//    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    DwarfShirt.Draw();
//
//    // Draw DwarfLeather ===========================================
//    DwarfLeather_PS.Bind_ALL(immediateContext);
//    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());
//
//    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    DwarfLeather.Draw();
//
//    // Draw DwarfArmor ===========================================
//    DwarfArmor_PS.Bind_ALL(immediateContext);
//    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());
//
//    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    DwarfArmor.Draw();
//
//    // Draw DwarfAxe ===========================================
//    DwarfAxe_PS.Bind(immediateContext);
//    DwarfAxe_PS.BindShaderResources_1(immediateContext);
//    mtranlsation = XMMatrixTranslation(0.0f, 1.4f, -0.33f);
//    mscale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
//    XMMATRIX axe_rotation = XMMatrixRotationRollPitchYaw(-35.0f * (XM_PI / 180.0f), 80.0f * (XM_PI / 180.0f), -5.0f * (XM_PI / 180.0f));
//    temp = mtranlsation * mrotation * DwarfBody.GetWorldMatrix();
//    temp = axe_rotation * temp;
//    temp = mscale * temp;
//    cb.mWorld = XMMatrixTranspose(temp);
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);
//
//    DwarfAxe.Draw();
//
//    // Draw HUD ===========================================
//    HUD_VS.Bind(immediateContext);
//    HUD_PS.Bind(immediateContext);
//    HUD_PS.BindShaderResources_1(immediateContext);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)HUD_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)HUD_VS.GetConstantBuffer(), 0);
//
//    HUD.Draw();
//
//    // Draw Smoke ===========================================
//    Smoke_VS.Bind(immediateContext);
//    Smoke_PS.Bind(immediateContext);
//    Smoke_PS.BindShaderResources_1(immediateContext);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)Smoke_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)Smoke_VS.GetConstantBuffer(), 0);
//
//    HUD.Draw();
//
//    // Draw Grid ========================================
//    if (DrawGrid)
//    {
//        solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer;
//        default_VS.Bind(immediateContext);
//        solid_PS.Bind(immediateContext);
//        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
//
//        grid.Draw();
//    }
//
//    bool vysnc = true;
//    swapChain->Present(vysnc, 0);
//}
//
//void DrawDebugScene()
//{
//    // rendering here (create function)
//    immediateContext->ClearRenderTargetView(RTV, Colors::Black);
//    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
//    // setup pipeline
//        // IA (Input Assembler)
//        // VS (Vertex Shader)
//        // RS (Rasterizer Stage)
//    immediateContext->RSSetViewports(1, &vPort);
//    // PS (Pixel Shader)
//    // OM (Output Merger)
//    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);
//
//
//    // upload matrices to video card
//        // Create and update a constant buffer (move variables from C++ to shaders)
//    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
//    HRESULT hr;
//
//    ConstantBuffer cb = {};
//
//    cb.mView = XMMatrixTranspose(cam.GetViewMatrix());
//
//    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
//    cb.mProjection = XMMatrixTranspose(cam.GetProjectionMatrix());
//
//    WVP wvp = {};
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    XMStoreFloat4x4(&wvp.sView, cb.mView);
//    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);
//
//    XMMATRIX temp_LtRotY = XMMatrixRotationY(-0.01f);
//    //pntLight.SetPosition(XMVector3Transform(pntLight.GetPositionVector(), temp_LtRotY));
//    //sptLight.SetPosition(XMVector3Transform(sptLight.GetPositionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightPos[0], dirLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[1], pntLight.GetPositionVector());
//    XMStoreFloat4(&wvp.LightPos[2], sptLight.GetPositionVector());
//
//    XMMATRIX temp_LtRotZ = XMMatrixRotationZ(-0.01f);
//    //dirLight.SetDirection(XMVector3Transform(dirLight.GetDirectionVector(), temp_LtRotZ));
//    //sptLight.SetDirection(XMVector3Transform(sptLight.GetConeDirectionVector(), temp_LtRotY));
//    XMStoreFloat4(&wvp.LightDir[0], dirLight.GetDirectionVector());
//    XMStoreFloat4(&wvp.LightDir[1], sptLight.GetConeDirectionVector());
//
//    XMStoreFloat4(&wvp.LightColor[0], dirLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[1], pntLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.LightColor[2], sptLight.GetLightColorVector());
//    XMStoreFloat4(&wvp.CamPos, cam.GetPositionVector());
//    //wvp.totalTime.x = (float)gTimer.deltaTime / 1000.0f;
//    auto jointSize = (run_anim.IsPlaying()) ? run_anim.TweenJoints.size() : run_anim.GetCurrentKeyframe()->joints.size();
//    for (size_t i = 0; i < jointSize; ++i)
//    {
//        DirectX::XMMATRIX mat;
//        if (run_anim.IsPlaying())
//            mat = XMLoadFloat4x4(&run_anim.TweenJoints[i].global_xform);
//        else
//            mat = XMLoadFloat4x4(&run_anim.GetCurrentKeyframe()->joints[i].global_xform);
//        DirectX::XMMATRIX invMat = XMLoadFloat4x4(&run_anim.Frames[0].joints[i].inv_xform);
//        invMat = XMMatrixTranspose(invMat * mat);
//        XMStoreFloat4x4(&wvp.SkinMat[i], invMat);
//    }
//
//    //======================================================================================================================
//
//    // Draw Point to Quad ==================================
//    if (DrawQuad)
//    {
//        solid_PS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;
//        gs_VS.Bind(immediateContext);
//        solid_PS.Bind(immediateContext);
//        pntToQuad_GS.Bind(immediateContext);
//        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0);
//
//        point.Draw();
//        immediateContext->GSSetShader(nullptr, nullptr, 0);
//        solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer; // for grid and others
//    }
//
//    // Draw Skybox =====================================
//    if (true)
//    {
//        immediateContext->RSSetState(RSCullNone); // turn back face culling off
//        immediateContext->OMSetDepthStencilState(DSLessEqual, 0); // draw skybox everywhere that is not drawn on
//        skybox_PS.Bind(immediateContext);
//        skybox_VS.Bind(immediateContext);
//        skybox_PS.BindShaderResources_1(immediateContext);
//        cb.mWorld = XMMatrixTranspose(XMMatrixTranslationFromVector(cam.GetPositionVector()));
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0);
//
//        skybox.Mesh.Draw();
//        immediateContext->RSSetState(nullptr);
//        immediateContext->OMSetDepthStencilState(nullptr, 0);
//    }
//
//    // Draw fbx mesh
//    //default_PS.Bind(immediateContext);
//    //default_PS.BindShaderResources(immediateContext);
//    anim_VS.Bind(immediateContext);
//    anim_PS.Bind(immediateContext);
//    anim_PS.BindShaderResources(immediateContext);
//
//    cb.mWorld = XMMatrixTranspose(BattleMage.GetWorldMatrix());
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0);
//    BattleMage.Draw();
//
//    // Draw Point Light
//    //solid_PS.Bind(immediateContext);
//    //cb.mWorld = XMMatrixTranspose(XMMatrixScaling(0.05f, 0.05f, 0.05f) * pntLight.GetWorldMatrix());
//
//    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    //// send to Card
//    //hr = immediateContext->Map((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    //memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    //immediateContext->Unmap((ID3D11Resource*)anim_VS.GetConstantBuffer(), 0);
//    //BattleMage.Draw();
//
//
//    default_VS.Bind(immediateContext);
//    solid_PS.Bind(immediateContext);
//    // Draw Grid ========================================
//    immediateContext->RSSetState(RSAALLines);
//    if (!DrawGrid)
//    {
//        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//        // send to Card
//        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
//        grid.Draw();
//    }
//    // Draw Debug_renderer ========================================
//    immediateContext->OMSetDepthStencilState(DSNoDepth, 0);
//    Mesh<VERTEX> temp = Mesh<VERTEX>(myDevice, immediateContext,
//        end::debug_renderer::get_line_verts(),
//        (int)end::debug_renderer::get_line_vert_capacity(),
//        D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
//
//    cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());
//
//    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
//    // send to Card
//    hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
//    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
//    immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
//    temp.Draw();
//
//    end::debug_renderer::clear_lines();
//    immediateContext->RSSetState(nullptr);
//    immediateContext->OMSetDepthStencilState(nullptr, 0);
//
//
//
//    // change 1 to 0 vsync
//    bool vysnc = true;
//    swapChain->Present(vysnc, 0);
//}
#pragma endregion