// JustinGXEngine.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "JustinGXEngine.h"

#include "Cube.h"
#include "Particle.h"
#include "Emitter.h"
#include "pools.h"
#include "Camera.h"
#include "Light.h"
#include "Shaders.h"
#include "Time.h"

#include "debug_renderer.h"

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

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <iostream>
#pragma comment(lib, "d3d11.lib")

#define RAND_FLT(min, max)  (min + (rand() / (float)RAND_MAX) * (max - min))

using namespace DirectX;

// for init
ID3D11Device* myDevice = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;

// for drawing
ID3D11RenderTargetView* RTV = nullptr;
D3D11_VIEWPORT vPort;
float aspectRatio = 1;

struct ConstantBuffer
{
    XMMATRIX mWorld;
    XMMATRIX mView;
    XMMATRIX mProjection;
    XMFLOAT4 LightPos[3];
    XMFLOAT4 LightDir[3];
    XMFLOAT4 LightColor[3];
};

 //storage value for math
struct WVP
{
    XMFLOAT4X4 sWorld;
    XMFLOAT4X4 sView;
    XMFLOAT4X4 sProjection;
    XMFLOAT4 LightPos[3];
    XMFLOAT4 LightDir[3];
    XMFLOAT4 LightColor[3];
    XMFLOAT4 CamPos;
    XMFLOAT4 totalTime;
};

XMVECTOR LightPositions[3] =
{
    {-0.577f, 0.577f, -0.577f, 1.0f}, // directional
    {-10.0f, 5.0f, 0.0f, 1.0f}, // point
    {5.0f, 5.0f, 0.0f, 1.0f} // spot
};
XMVECTOR LightDirs[3] =
{
    {-0.577f, 0.577f, -0.577f, 1.0f}, // directional
    {-10.0f, 5.0f, 0.0f, 1.0f}, // point
    {1.0f, -1.0f, 0.0f, 1.0f} // spot
};
XMVECTOR LightColors[3] =
{
    {0.75f, 0.75f, 0.75f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f}
};

UINT numOfElements = 0;
UINT numOfVerts = 0;

// z buffer
ID3D11Texture2D* zBuffer = nullptr;
ID3D11DepthStencilView* zBufferView = nullptr;

ID3D11DepthStencilState* DSLessEqual = nullptr; // used to make sure skybox is always behind all the other geometry
ID3D11RasterizerState* RSCullNone = nullptr; // used to disable backface-culling
ID3D11BlendState* blendState = nullptr;

#ifdef _DEBUG
ID3D11Debug* debug = nullptr;
#endif

// Objects
Time gTimer;
Camera cam;
Mesh<VERTEX> grid;
Mesh<_OBJ_VERT_> DwarfBody;
Mesh<_OBJ_VERT_> DwarfShirt;
Mesh<_OBJ_VERT_> DwarfLeather;
Mesh<_OBJ_VERT_> DwarfArmor;
Mesh<_OBJ_VERT_> DwarfAxe;
Mesh<VERTEX> HUD;

Mesh<_OBJ_VERT_> planet_1;
Mesh<_OBJ_VERT_> planet_2;
Mesh<_OBJ_VERT_> planet_3;
Mesh<_OBJ_VERT_> moon;
Mesh<_OBJ_VERT_> talon;
Mesh<VERTEX> point;
Cube skybox;
DirectionalLight dirLight;
PointLight pntLight;
SpotLight sptLight;

Emitter sortEmitter;
end::Sorted_Pool_t<Particle, 256> sortPool;
Emitter emitters[4];
end::Pool_t<Particle, 1024> sharedPool;


Shaders::VertexShader advanced_VS;
Shaders::VertexShader default_VS;
Shaders::VertexShader skybox_VS;
Shaders::VertexShader gs_VS;
Shaders::PixelShader advanced_PS;
Shaders::PixelShader solid_PS;
Shaders::PixelShader skybox_PS;

Shaders::VertexShader HUD_VS;
Shaders::VertexShader Smoke_VS;
Shaders::PixelShader DwarfBody_PS;
Shaders::PixelShader DwarfShirt_PS;
Shaders::PixelShader DwarfLeather_PS;
Shaders::PixelShader DwarfArmor_PS;
Shaders::PixelShader DwarfAxe_PS;
Shaders::PixelShader HUD_PS;
Shaders::PixelShader Smoke_PS;

Shaders::PixelShader planet1_PS;
Shaders::PixelShader planet2_PS;
Shaders::PixelShader planet3_PS;
Shaders::PixelShader moon_PS;
Shaders::PixelShader talon_PS;
Shaders::GeometryShader pntToQuad_GS;

bool DrawQuad = false;
bool DrawGrid = false;



#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE       hInst;                                // current instance
HWND            hWnd = nullptr;
WCHAR           szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR           szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitDevice();
HRESULT             InitContent();
void                CleanUp();
void                CatchInput();
void                DrawSpaceScene();
void                DrawDwarfScene();
void                DrawDebugScene();
void                Update();

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    //	_CrtSetBreakAlloc(187);

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUSTINGXENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
        return FALSE;

    // TODO: Place code here.
    if (FAILED(InitDevice()))
    {
        CleanUp();
        return 0;
    }

    if (FAILED(InitContent()))
    {
        CleanUp();
        return 0;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JUSTINGXENGINE));

    MSG msg = { 0 };

    //RAWINPUTDEVICE rid;
    //rid.usUsagePage = 0x01;
    //rid.usUsage = 0x02;
    //rid.dwFlags = 0;
    //rid.hwndTarget = NULL;

    //if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
    //{
    //    // uh-oh
    //    return 0;
    //}

    // Main message loop:

    gTimer.Start();
    while (msg.message != WM_QUIT) //GetMessage(&msg, nullptr, 0, 0)) waits for message
    {
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Update();
        DrawDebugScene();
        //DrawSpaceScene();
        //DrawDwarfScene();
    }
    gTimer.Stop();
    // release all our D3D11 interfaces
    CleanUp();

#ifdef _DEBUG
    debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    debug->Release();
#endif

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JUSTINGXENGINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_JUSTINGXENGINE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    // find resize stuff
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}


HRESULT InitDevice()
{
    HRESULT hr = S_OK;

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
        return hr;

#ifdef _DEBUG
    hr = myDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
    //hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // Create Render Target View
    ID3D11Resource* backbuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(backbuffer), reinterpret_cast<void**>(&backbuffer));
    if (FAILED(hr))
        return hr;

    hr = myDevice->CreateRenderTargetView(backbuffer, NULL, &RTV);
    backbuffer->Release();
    if (FAILED(hr))
        return hr;


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
        return hr;

    // Create the depth stencil view
    //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    //descDSV.Format = descDepth.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.Texture2D.MipSlice = 0;
    hr = myDevice->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);
    if (FAILED(hr))
        return hr;

    // Setup viewport
    vPort.Width = static_cast<FLOAT>(width);
    vPort.Height = static_cast<FLOAT>(height);
    vPort.TopLeftX = vPort.TopLeftY = 0;
    vPort.MinDepth = 0;
    vPort.MaxDepth = 1;

    

//#ifdef _DEBUG
//    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//#endif

    return hr;
}

HRESULT InitContent()
{
    HRESULT hr = S_OK;

    // write, compile & load our shaders
    hr = advanced_VS.Initialize(myDevice, "./MeshVertexShader.cso", objLayoutDesc, ARRAYSIZE(objLayoutDesc), sizeof(WVP));

    hr = default_VS.Initialize(myDevice, "./VertexShader.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));

    hr = skybox_VS.Initialize(myDevice, "./SkyBox_VS.cso", cubeLayoutDesc, ARRAYSIZE(cubeLayoutDesc), sizeof(WVP));

    hr = gs_VS.Initialize(myDevice, "./Geo_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));

    hr = HUD_VS.Initialize(myDevice, "./HUD_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));

    hr = Smoke_VS.Initialize(myDevice, "./HUD_VS.cso", vertexInputLayoutDesc, ARRAYSIZE(vertexInputLayoutDesc), sizeof(WVP));

    hr = solid_PS.Initialize(myDevice, "./PS_Solid.cso", sizeof(WVP));
    solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer;
    
    hr = skybox_PS.Initialize(myDevice, "./SkyBox_PS.cso", sizeof(WVP)); // change to include texture
    hr = skybox_PS.InitShaderResources(myDevice, "./Assets/Textures/SunsetSkybox.dds");
    skybox_PS.ShaderConstantBuffer = skybox_VS.ShaderConstantBuffer;

    hr = pntToQuad_GS.Initialize(myDevice, "./PointToQuad_GS.cso", sizeof(WVP));
    pntToQuad_GS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;


    #pragma region Dwarf Shaders
    std::string textures[2] = { "./Assets/Textures/Dwarf/BodyTexture.dds", "./Assets/Textures/Dwarf/BodySpec.dds" };
    hr = DwarfBody_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    hr = DwarfBody_PS.InitShaderResources(myDevice, textures);
    DwarfBody_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    textures->clear(); 
    textures[0] = "./Assets/Textures/Dwarf/ShirtTexture.dds";
    textures[1] = "./Assets/Textures/Dwarf/ShirtSpec.dds";
    hr = DwarfShirt_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    hr = DwarfShirt_PS.InitShaderResources(myDevice, textures);
    DwarfShirt_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    textures->clear();
    textures[0] = "./Assets/Textures/Dwarf/LeatherTexture.dds";
    textures[1] = "./Assets/Textures/Dwarf/LeatherSpec.dds";
    hr = DwarfLeather_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    hr = DwarfLeather_PS.InitShaderResources(myDevice, textures);
    DwarfLeather_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    textures->clear();
    textures[0] = "./Assets/Textures/Dwarf/ArmorTexture.dds";
    textures[1] =  "./Assets/Textures/Dwarf/ArmorSpec.dds";
    hr = DwarfArmor_PS.Initialize(myDevice, "./Specular_PS.cso", sizeof(WVP));
    hr = DwarfArmor_PS.InitShaderResources(myDevice, textures);
    DwarfArmor_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
    hr = DwarfAxe_PS.Initialize(myDevice, "./SkyBoxReflection_PS.cso", sizeof(WVP));
    hr = DwarfAxe_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LostValley.dds");
    DwarfAxe_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    hr = HUD_PS.Initialize(myDevice, "./HUD_PS.cso", sizeof(WVP));
    hr = HUD_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LoadingText.dds");
    HUD_PS.ShaderConstantBuffer = HUD_VS.ShaderConstantBuffer;
    hr = Smoke_PS.Initialize(myDevice, "./Smoke_PS.cso", sizeof(WVP));
    hr = Smoke_PS.InitShaderResources(myDevice, "./Assets/Textures/Dwarf/LoadingSmoke.dds");
    Smoke_PS.ShaderConstantBuffer = Smoke_VS.ShaderConstantBuffer;
#pragma endregion

    #pragma region SpaceScene Shaders
    hr = planet1_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
    hr = planet1_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet_Diffuse.dds");
    planet1_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    hr = planet2_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
    hr = planet2_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet2_Diffuse.dds");
    planet2_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    hr = planet3_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
    hr = planet3_PS.InitShaderResources(myDevice, "./Assets/Textures/RT_2D_Planet4_Diffuse.dds");
    planet3_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    hr = moon_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
    hr = moon_PS.InitShaderResources(myDevice, "./Assets/Textures/moon_Diffuse.dds");
    moon_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;

    hr = talon_PS.Initialize(myDevice, "./SingleTexture_PS.cso", sizeof(WVP)); // change to include texture
    hr = talon_PS.InitShaderResources(myDevice, "./Assets/Textures/defender.dds");
    talon_PS.ShaderConstantBuffer = advanced_VS.ShaderConstantBuffer;
#pragma endregion

    #pragma region Dwarf Meshes
    std::vector<_OBJ_VERT_> verts;
    std::vector<int> indices;
    // CREATE DWARFBODY
    numOfVerts = ARRAYSIZE(DwarfBody_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        verts.push_back(DwarfBody_data[i]);
    numOfElements = ARRAYSIZE(DwarfBody_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        indices.push_back(DwarfBody_indicies[i]);
    DwarfBody = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    DwarfBody.SetPosition(-1.0f, 0.0f, 0.0f);
    DwarfBody.SetRotation(0.0f, XM_PI, 0.0f);

    verts.clear(); verts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();
    // CREATE DWARFSHIRT
    numOfVerts = ARRAYSIZE(DwarfShirt_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        verts.push_back(DwarfShirt_data[i]);
    numOfElements = ARRAYSIZE(DwarfShirt_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        indices.push_back(DwarfShirt_indicies[i]);
    DwarfShirt = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    verts.clear(); verts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();
    // CREATE DWARFLEATHER
    numOfVerts = ARRAYSIZE(DwarfLeather_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        verts.push_back(DwarfLeather_data[i]);
    numOfElements = ARRAYSIZE(DwarfLeather_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        indices.push_back(DwarfLeather_indicies[i]);
    DwarfLeather = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    verts.clear(); verts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();
    // CREATE DwarfArmor
    numOfVerts = ARRAYSIZE(DwarfArmor_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        verts.push_back(DwarfArmor_data[i]);
    numOfElements = ARRAYSIZE(DwarfArmor_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        indices.push_back(DwarfArmor_indicies[i]);
    DwarfArmor = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    verts.clear(); verts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();

    // CREATE DwarfAxe
    numOfVerts = ARRAYSIZE(DwarfAxe_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        verts.push_back(DwarfAxe_data[i]);
    numOfElements = ARRAYSIZE(DwarfAxe_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        indices.push_back(DwarfAxe_indicies[i]);
    DwarfAxe = Mesh<_OBJ_VERT_>(myDevice, immediateContext, verts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    DwarfAxe.SetPosition(-1.0f, 0.0f, 0.0f);
    DwarfAxe.SetRotation(0.0f, 180.0f * (XM_PI / 180.0f), 0.0f);

    verts.clear(); verts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();

    // CREATE HUD
    std::vector<VERTEX> HUDverts;
    HUDverts =
    {
        VERTEX({-1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}),
        VERTEX({1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}),
        VERTEX({-1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}),
        VERTEX({1.0f, -1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f})
    };
    indices =
    {
        0,1,2,
        2,1,3
    };
    HUD = Mesh<VERTEX>(myDevice, immediateContext, HUDverts, indices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HUDverts.clear(); HUDverts.shrink_to_fit();
    indices.clear(); indices.shrink_to_fit();
#pragma endregion
    
    #pragma region SpaceScene Meshes
    // PLANET_1 =================================
    std::vector<_OBJ_VERT_> planetVerts;
    numOfVerts = ARRAYSIZE(Planet_1_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        planetVerts.push_back(Planet_1_data[i]);
    std::vector<int> planetIndices;
    numOfElements = ARRAYSIZE(Planet_1_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        planetIndices.push_back(Planet_1_indicies[i]);
    planet_1 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // PLANET_2 =================================
    planetVerts.clear();
    planetIndices.clear();
    numOfVerts = ARRAYSIZE(Planet_2_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        planetVerts.push_back(Planet_2_data[i]);
    numOfElements = ARRAYSIZE(Planet_2_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        planetIndices.push_back(Planet_2_indicies[i]);
    planet_2 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // PLANET_3 =================================
    planetVerts.clear();
    planetIndices.clear();
    numOfVerts = ARRAYSIZE(Planet_3_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        planetVerts.push_back(Planet_3_data[i]);
    numOfElements = ARRAYSIZE(Planet_3_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        planetIndices.push_back(Planet_3_indicies[i]);
    planet_3 = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // MOON ====================================
    planetVerts.clear();
    planetIndices.clear();
    numOfVerts = ARRAYSIZE(Moon_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        planetVerts.push_back(Moon_data[i]);
    numOfElements = ARRAYSIZE(Moon_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        planetIndices.push_back(Moon_indicies[i]);
    moon = Mesh<_OBJ_VERT_>(myDevice, immediateContext, planetVerts, planetIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // TALON ===================================
    std::vector<_OBJ_VERT_> talonVerts;
    numOfVerts = ARRAYSIZE(talon_data);
    for (size_t i = 0; i < numOfVerts; ++i)
        talonVerts.push_back(talon_data[i]);
    std::vector<int> talonIndices;
    numOfElements = ARRAYSIZE(talon_indicies);
    for (size_t i = 0; i < numOfElements; ++i)
        talonIndices.push_back(talon_indicies[i]);
    talon = Mesh<_OBJ_VERT_>(myDevice, immediateContext, talonVerts, talonIndices, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion

    std::vector<VERTEX> pnt_Vert;
    pnt_Vert.push_back(VERTEX({ 0.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f }));
    point = Mesh<VERTEX>(myDevice, immediateContext, pnt_Vert, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    skybox.cube_mesh = Mesh<VERTEX_BASIC>(myDevice, immediateContext, skybox._vertexList, skybox._indicesList, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Define Particles
    std::vector<VERTEX> sparkVerts =
    {
        VERTEX({ 0.0f, 0.25f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }),
        VERTEX({ 0.0f, -0.25f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f, 1.0f }),
    };
    for (uint16_t i = 0; i < sortPool.capacity(); ++i)
    {
        sortPool[i].Mesh = Mesh<VERTEX>(myDevice, immediateContext, 
            sparkVerts.data(), 
            (int)sparkVerts.size(), 
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        sortPool[i].Velocity = { RAND_FLT(-5.0f, 5.0f), 15.0f, RAND_FLT(-5.0f, 5.0f), 0.0f };
        sortPool[i].Lifetime = 3.0f;
    }

    emitters[0].SetSpawnPosition(-9.0f, 0.0f, 9.0f);
    emitters[0].Spawn_Color = { 0.0f, 1.0f, 0.0f, 1.0f };
    emitters[1].SetSpawnPosition(-9.0f, 0.0f, -9.0f);
    emitters[1].Spawn_Color = { 0.0f, 0.0f, 1.0f, 1.0f };
    emitters[2].SetSpawnPosition(9.0f, 0.0f, 9.0f);
    emitters[2].Spawn_Color = { 1.0f, 1.0f, 0.0f, 1.0f };
    emitters[3].SetSpawnPosition(9.0f, 0.0f, -9.0f);
    emitters[3].Spawn_Color = { 0.0f, 1.0f, 1.0f, 1.0f };
    for (size_t i = 0; i < ARRAYSIZE(emitters); ++i)
    {
        sparkVerts =
        {
            VERTEX({ 0.0f, 0.25f, 0.0f, 1.0f }, emitters[i].Spawn_Color),
            VERTEX({ 0.0f, -0.25f, 0.0f, 1.0f }, emitters[i].Spawn_Color),
        };
        //int n = sizeof(sharedPool) / sizeof(sharedPool[0]);
        size_t length = emitters[i].indices.capacity() * (i + 1);
        for (uint16_t j = (uint16_t)(i * emitters[i].indices.capacity()); j < length; ++j)
        {
            sharedPool[j].Mesh = Mesh<VERTEX>(myDevice, immediateContext, sparkVerts.data(), (int)sparkVerts.size(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            sharedPool[j].Mesh.SetPosition(emitters[i].GetSpawnPositionVector());
            sharedPool[j].Velocity = { RAND_FLT(-5.0f, 5.0f), 15.0f, RAND_FLT(-5.0f, 5.0f), 0.0f };
            sharedPool[j].Lifetime = 3.0f;
        }
    }

    cam.SetPosition(0.0f, 5.0f, -15.0f);

    // initialize Directional Light
    dirLight.SetPosition(-20.0f, 20.0f, 0.0f);
    dirLight.SetDirection(-1.0f, 0.0f, -0.2f);
    dirLight.SetLightColor(0.75f, 0.75f, 0.75f, 1.0f);
    dirLight.SetAmbientTerm(0.3f);
    // initialize Point Light
    pntLight.SetPosition(-5.0f, 3.0f, -1.0f);
    pntLight.SetLightColor(1.0f, 1.0f, 1.0f, 1.0f);
    pntLight.SetAmbientTerm(0.9f);
    pntLight.SetPointRadius(35.0f);
    // initialize Spot Light
    sptLight.SetPosition(0.0f, 12.0f, 0.0f);
    sptLight.SetDirection(0.0f, -1.0f, 0.0f);
    sptLight.SetLightColor(0.0f, 1.0f, 0.0f, 1.0f);
    sptLight.SetAmbientTerm(0.1f);
    sptLight.SetOuterInnerConeRatios(0.5f, 0.8f);

    // TEXTURING ===============================================================================================

    D3D11_RASTERIZER_DESC rd;
    ZeroMemory(&rd, sizeof(rd));
    rd.CullMode = D3D11_CULL_NONE;
    rd.FillMode = D3D11_FILL_SOLID;
    hr = myDevice->CreateRasterizerState(&rd, &RSCullNone);

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    hr = myDevice->CreateDepthStencilState(&depthStencilDesc, &DSLessEqual);

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
    myDevice->CreateBlendState(&blendDesc, &blendState);


#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif
    
    return hr;
}

/// <summary>
/// Cleans up and releases any objects that were created
/// </summary>
void CleanUp()
{
    if (immediateContext) immediateContext->ClearState();

    if (RTV) RTV->Release();
    if (zBuffer) zBuffer->Release();
    if (zBufferView) zBufferView->Release();
    if (RSCullNone) RSCullNone->Release();
    if (DSLessEqual) DSLessEqual->Release();
    if (blendState) blendState->Release();
    if (swapChain) swapChain->Release();
    if (immediateContext) immediateContext->Release();
    if (myDevice) myDevice->Release();
}

/// <summary>
/// Catches the keyboard and mouse input
/// </summary>
void CatchInput()
{
    static Time uTimer;
    uTimer.GetElapsedMilliseconds();
    uTimer.Restart();

    const float cameraSpeed = 0.002f;

    POINT curr_point = { 0,0 };
    POINT delta_point = { 0,0 };

    GetCursorPos(&curr_point); // grab the curr every frame

    static POINT prev_point = curr_point; // initialize once

    // calc delta of mouse pos with the pos of the previous frame
    delta_point.x = curr_point.x - prev_point.x;
    delta_point.y = curr_point.y - prev_point.y;

    prev_point = curr_point; // keep the current pos of the current frame to use in the next frame

    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
    {
        cam.UpdateRotation(static_cast<float>(delta_point.y) * 0.005f, static_cast<float>(delta_point.x) * 0.005f, 0.0f);
    }
    if (GetAsyncKeyState('W') & 0x8000)
    {
        cam.UpdatePosition(cam.GetForwardVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        cam.UpdatePosition(cam.GetLeftVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        cam.UpdatePosition(cam.GetBackwardVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        cam.UpdatePosition(cam.GetRightVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        cam.UpdatePosition(cam.GetUpVector() * cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        cam.UpdatePosition(cam.GetUpVector() * -cameraSpeed * (float)uTimer.deltaTime);
    }
    if (GetAsyncKeyState('Q') & 0x0001)
    {
        DrawQuad = !DrawQuad;
    }
    if (GetAsyncKeyState('G') & 0x0001)
    {
        DrawGrid = !DrawGrid;
    }
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //HDC hdc;
    //RECT rcClient;                 // client area rectangle 
    //POINT ptClientUL;              // client upper left corner 
    //POINT ptClientLR;              // client lower right corner 
    ////static POINTS mouseCoords;        // beginning point
    //static POINT prev_point = { 0,0 };
    //static POINT curr_point = { 0,0 };
    //static POINT delta_point = { 0,0 };


    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    //case WM_RBUTTONDOWN:
    //{
    //    // Capture mouse input. 

    //    SetCapture(hWnd);

    //    // Retrieve the screen coordinates of the client area, 
    //    // and convert them into client coordinates. 

    //    GetClientRect(hWnd, &rcClient);
    //    ptClientUL.x = rcClient.left;
    //    ptClientUL.y = rcClient.top;

    //    // Add one to the right and bottom sides, because the 
    //    // coordinates retrieved by GetClientRect do not 
    //    // include the far left and lowermost pixels. 

    //    ptClientLR.x = rcClient.right + 1;
    //    ptClientLR.y = rcClient.bottom + 1;
    //    ClientToScreen(hWnd, &ptClientUL);
    //    ClientToScreen(hWnd, &ptClientLR);

    //    // Copy the client coordinates of the client area 
    //    // to the rcClient structure. Confine the mouse cursor 
    //    // to the client area by passing the rcClient structure 
    //    // to the ClipCursor function. 

    //    SetRect(&rcClient, ptClientUL.x, ptClientUL.y,
    //        ptClientLR.x, ptClientLR.y);
    //    ClipCursor(&rcClient); // confines the cursor within the client area

    //    // Convert the cursor coordinates into a POINTS 
    //    // structure, which defines the beginning point of the 
    //    // line drawn during a WM_MOUSEMOVE message. 

    //    //curr_point.x = LOWORD(lParam);
    //    //curr_point.y = HIWORD(lParam);
    //    return 0;
    //}
    ////case WM_INPUT:
    ////    {
    ////        UINT dataSize;
    ////        GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));
    ////        if (dataSize > 0)
    ////        {
    ////            std::unique_ptr<BYTE[]> rawData = std::make_unique<BYTE[]>(dataSize);
    ////            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawData.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
    ////            {
    ////                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawData.get());
    ////                if (raw->header.dwType == RIM_TYPEMOUSE) // check if raw data is a mouse data type
    ////                {
    ////                    mouseCoords.x = raw->data.mouse.lLastX;
    ////                    mouseCoords.y = raw->data.mouse.lLastY;
    ////                }
    ////            }
    ////        }
    ////
    ////
    ////        return DefWindowProc(hWnd, message, wParam, lParam);
    ////    }
    ////case WM_MOUSEMOVE:

    ////    // When moving the mouse, the user must hold down 
    ////    // the left mouse button to rotate the camera. 
    ////    //if (wParam & MK_RBUTTON)
    ////    //{
    ////    //    prev_point = curr_point;
    ////    //    curr_point.x = LOWORD(lParam);
    ////    //    curr_point.y = HIWORD(lParam);
    ////    //    //GetCursorPos(&curr_point);
    ////    //    // calc delta of mouse pos
    ////    //    delta_point.x = curr_point.x - prev_point.x;
    ////    //    delta_point.y = curr_point.y - prev_point.y;

    ////    //    cam.UpdateRotation(static_cast<float>(delta_point.y) * 0.001f, static_cast<float>(delta_point.x) * 0.001f, 0.0f);
    ////    //}

    ////    break;
    //case WM_RBUTTONUP:

    //    // The user has finished drawing the line. Reset the 
    //    // previous line flag, release the mouse cursor, and 
    //    // release the mouse capture. 

    //    ClipCursor(NULL);
    //    ReleaseCapture();
    //    return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void DrawSpaceScene()
{
    // rendering here (create function)
    immediateContext->ClearRenderTargetView(RTV, Colors::DarkBlue);
    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    // setup pipeline
        // IA (Input Assembler)
        // VS (Vertex Shader)
        // RS (Rasterizer Stage)
    immediateContext->RSSetViewports(1, &vPort);
        // PS (Pixel Shader)
        // OM (Output Merger)
    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);


    // upload matrices to video card
        // Create and update a constant buffer (move variables from C++ to shaders)
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    HRESULT hr;
    
    //static float rot = 0; rot += 0.005f;
    ConstantBuffer cb = {};

    cb.mView = XMMatrixTranspose( cam.GetViewMatrix() );

    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
    cb.mProjection = XMMatrixTranspose( cam.GetProjectionMatrix() );

    WVP wvp = {};
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    XMStoreFloat4x4(&wvp.sView, cb.mView);
    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);

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

    //======================================================================================================================

    // Draw Planet_1 ===========================================
    advanced_VS.Bind(immediateContext);
    planet1_PS.Bind(immediateContext);
    planet1_PS.BindShaderResources_1(immediateContext);
    XMMATRIX mtranlsation = XMMatrixTranslation(0.0f, 0.0f, 60.0f);
    XMMATRIX mscale = XMMatrixScaling(0.023f, 0.023f, 0.023f);
    XMMATRIX mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 0.5f);
    XMMATRIX Planet1m = mscale * mtranlsation * mrotationY;
    cb.mWorld = XMMatrixTranspose(Planet1m);

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    planet_1.Draw();

    // Draw Planet_2 ===========================================
    planet2_PS.Bind(immediateContext);
    planet2_PS.BindShaderResources_1(immediateContext);

    mtranlsation = XMMatrixTranslation(0.0f, 0.0f, -220.0f);
    mscale = XMMatrixScaling(0.2f, 0.2f, 0.2f);
    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 0.35f);
    XMMATRIX Planet2m = mscale * mtranlsation * mrotationY;
    cb.mWorld = XMMatrixTranspose(Planet2m);

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    planet_2.Draw();

    // Draw Planet_3 ===========================================
    planet3_PS.Bind(immediateContext);
    planet3_PS.BindShaderResources_1(immediateContext);

    mtranlsation = XMMatrixTranslation(-12.0f, 0.0f, 0.0f);
    mscale = XMMatrixScaling(0.010f, 0.010f, 0.010f);
    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * 1.1f);
    XMMATRIX Planet3m = mscale * mtranlsation * mrotationY ;
    cb.mWorld = XMMatrixTranspose(Planet3m);

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    planet_3.Draw();

    // Draw Moon ===========================================
    moon_PS.Bind(immediateContext);
    moon_PS.BindShaderResources_1(immediateContext);

    mtranlsation = XMMatrixTranslation(800.0f, 0.0f, 0.0f);
    mscale = XMMatrixScaling(10.0f, 10.0f, 10.0f);
    mrotationY = XMMatrixRotationY(((float)gTimer.deltaTime / 1000.0f) * XM_PI);
    XMMATRIX Moonm = mscale * mtranlsation * mrotationY;
    cb.mWorld = XMMatrixTranspose(Moonm * Planet1m);

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    moon.Draw();

    // Draw Ship =======================================
    talon_PS.Bind(immediateContext);
    talon_PS.BindShaderResources_1(immediateContext);
    cb.mWorld = XMMatrixTranspose(XMMatrixTranslation(0.0f, -0.5f, 2.0f) * cam.GetWorldMatrix());

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    talon.Draw();

    // Draw Point to Quad ==================================
    if (DrawQuad)
    {
        solid_PS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;
        gs_VS.Bind(immediateContext);
        solid_PS.Bind(immediateContext);
        pntToQuad_GS.Bind(immediateContext);
        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0);

        point.Draw();
        immediateContext->GSSetShader(nullptr, nullptr, 0);
    }

    // Draw Skybox =====================================
    immediateContext->RSSetState(RSCullNone); // turn back face culling off
    immediateContext->OMSetDepthStencilState(DSLessEqual, 0); // draw skybox everywhere that is not drawn on
    skybox_VS.Bind(immediateContext);
    skybox_PS.Bind(immediateContext);
    skybox_PS.BindShaderResources_1(immediateContext);
    cb.mWorld = XMMatrixTranspose(XMMatrixTranslationFromVector(cam.GetPositionVector()));

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0);

    skybox.cube_mesh.Draw();
    immediateContext->RSSetState(nullptr);
    immediateContext->OMSetDepthStencilState(nullptr, 0);

    // Draw Grid ========================================

    if (DrawGrid)
    {
        //if (solid_PS.ShaderConstantBuffer != default_VS.ShaderConstantBuffer)
        //{
        //    solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer;
        //}

        default_VS.Bind(immediateContext);
        solid_PS.Bind(immediateContext);
        std::vector<VERTEX> lines = MakeGrid(20.0f, 25);
        //std::vector<VERTEX> lines = MakeColorGrid(20.0f, 25, (float)gTimer.deltaTime);
        //grid = Mesh<VERTEX>(myDevice, immediateContext, lines, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
        grid.Draw();
    }

    // change 1 to 0 vsync
    bool vysnc = true;
    swapChain->Present(vysnc, 0);
}

void DrawDwarfScene()
{
    // rendering here (create function)
    immediateContext->ClearRenderTargetView(RTV, Colors::Black);
    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    // setup pipeline
        // IA (Input Assembler)
        // VS (Vertex Shader)
        // RS (Rasterizer Stage)
    immediateContext->RSSetViewports(1, &vPort);
        // PS (Pixel Shader)
        // OM (Output Merger)
    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);
    immediateContext->OMSetBlendState(blendState, 0, 0xffffffff);

    // upload matrices to video card
        // Create and update a constant buffer (move variables from C++ to shaders)
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    HRESULT hr;

    XMMATRIX temp2 = XMMatrixTranslation(1.5f, 0, 0);
    ConstantBuffer cb = {};

    cb.mView = XMMatrixTranspose(cam.GetViewMatrix());

    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
    cb.mProjection = XMMatrixTranspose(cam.GetProjectionMatrix());

    WVP wvp = {};
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    XMStoreFloat4x4(&wvp.sView, cb.mView);
    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);

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
    wvp.totalTime.x = GetTickCount64() / 1000.f;

    //======================================================================================================================
    static float time = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    time = (timeCur - timeStart) / 1000.0f;

    // Draw DwarfBody ===========================================
    advanced_VS.Bind(immediateContext);
    DwarfBody_PS.Bind_ALL(immediateContext);
    XMMATRIX temp;
    XMMATRIX mtranlsation = XMMatrixTranslation(0.0f, 0.0f, 60.0f);
    XMMATRIX mscale = XMMatrixScaling(0.023f, 0.023f, 0.023f);
    XMMATRIX mrotation = XMMatrixRotationY((6.0f * (XM_PI / 180.0f)) * (float)gTimer.deltaTime);
    cb.mWorld = XMMatrixTranspose(mrotation * DwarfBody.GetWorldMatrix());

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    DwarfBody.Draw();

    // Draw DwarfShirt ===========================================
    DwarfShirt_PS.Bind_ALL(immediateContext);
    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());

    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    DwarfShirt.Draw();

    // Draw DwarfLeather ===========================================
    DwarfLeather_PS.Bind_ALL(immediateContext);
    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());

    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    DwarfLeather.Draw();

    // Draw DwarfArmor ===========================================
    DwarfArmor_PS.Bind_ALL(immediateContext);
    //cb.mWorld = XMMatrixTranspose(DwarfBody.GetWorldMatrix());

    //XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    DwarfArmor.Draw();

    // Draw DwarfAxe ===========================================
    DwarfAxe_PS.Bind(immediateContext);
    DwarfAxe_PS.BindShaderResources_1(immediateContext);
    mtranlsation = XMMatrixTranslation(0.0f, 1.4f, -0.33f);
    mscale = XMMatrixScaling(0.3f, 0.3f, 0.3f);
    XMMATRIX axe_rotation = XMMatrixRotationRollPitchYaw(-35.0f * (XM_PI / 180.0f) , 80.0f * (XM_PI / 180.0f), -5.0f * (XM_PI / 180.0f));
    temp = mtranlsation * mrotation * DwarfBody.GetWorldMatrix();
    temp = axe_rotation * temp;
    temp = mscale * temp;
    cb.mWorld = XMMatrixTranspose(temp);

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)advanced_VS.GetConstantBuffer(), 0);

    DwarfAxe.Draw();

    // Draw HUD ===========================================
    HUD_VS.Bind(immediateContext);
    HUD_PS.Bind(immediateContext);
    HUD_PS.BindShaderResources_1(immediateContext);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)HUD_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)HUD_VS.GetConstantBuffer(), 0);

    HUD.Draw();

    // Draw Smoke ===========================================
    Smoke_VS.Bind(immediateContext);
    Smoke_PS.Bind(immediateContext);
    Smoke_PS.BindShaderResources_1(immediateContext);
    // send to Card
    hr = immediateContext->Map((ID3D11Resource*)Smoke_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap((ID3D11Resource*)Smoke_VS.GetConstantBuffer(), 0);

    HUD.Draw();

    // Draw Grid ========================================
    if (DrawGrid)
    {
        solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer;
        default_VS.Bind(immediateContext);
        solid_PS.Bind(immediateContext);
        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);

        grid.Draw();
    }

    bool vysnc = true;
    swapChain->Present(vysnc, 0);
}

void DrawDebugScene()
{
    // rendering here (create function)
    immediateContext->ClearRenderTargetView(RTV, Colors::Black);
    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1.0f, 0);
    // setup pipeline
        // IA (Input Assembler)
        // VS (Vertex Shader)
        // RS (Rasterizer Stage)
    immediateContext->RSSetViewports(1, &vPort);
    // PS (Pixel Shader)
    // OM (Output Merger)
    immediateContext->OMSetRenderTargets(1, &RTV, zBufferView);


    // upload matrices to video card
        // Create and update a constant buffer (move variables from C++ to shaders)
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    HRESULT hr;

    ConstantBuffer cb = {};

    cb.mView = XMMatrixTranspose(cam.GetViewMatrix());

    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
    cb.mProjection = XMMatrixTranspose(cam.GetProjectionMatrix());

    WVP wvp = {};
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    XMStoreFloat4x4(&wvp.sView, cb.mView);
    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);

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

    //======================================================================================================================

    // Draw Point to Quad ==================================
    if (DrawQuad)
    {
        solid_PS.ShaderConstantBuffer = gs_VS.ShaderConstantBuffer;
        gs_VS.Bind(immediateContext);
        solid_PS.Bind(immediateContext);
        pntToQuad_GS.Bind(immediateContext);
        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)gs_VS.GetConstantBuffer(), 0);

        point.Draw();
        immediateContext->GSSetShader(nullptr, nullptr, 0);
        solid_PS.ShaderConstantBuffer = default_VS.ShaderConstantBuffer; // for grid and others
    }

    // Draw Skybox =====================================
    if (false)
    {
        immediateContext->RSSetState(RSCullNone); // turn back face culling off
        immediateContext->OMSetDepthStencilState(DSLessEqual, 0); // draw skybox everywhere that is not drawn on
        skybox_PS.Bind(immediateContext);
        skybox_VS.Bind(immediateContext);
        skybox_PS.BindShaderResources_1(immediateContext);
        cb.mWorld = XMMatrixTranspose(XMMatrixTranslationFromVector(cam.GetPositionVector()));

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)skybox_VS.GetConstantBuffer(), 0);

        skybox.cube_mesh.Draw();
        immediateContext->RSSetState(nullptr);
        immediateContext->OMSetDepthStencilState(nullptr, 0);
    }

    // Draw Grid ========================================
    default_VS.Bind(immediateContext);
    solid_PS.Bind(immediateContext);
    if (!DrawGrid)
    {
        grid = Mesh<VERTEX>(myDevice, immediateContext, 
            end::debug_renderer::get_line_verts(), 
            (int)end::debug_renderer::get_line_vert_capacity(), 
            D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        cb.mWorld = XMMatrixTranspose(XMMatrixIdentity());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
        grid.Draw();

        end::debug_renderer::clear_lines();
    }
    

    // Draw Particles
    for (uint16_t i = 0; i < sortEmitter.indices.size(); ++i) // draw each particle that is active
    {
        cb.mWorld = XMMatrixTranspose(sortPool[i].Mesh.GetWorldMatrix());

        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
        sortPool[i].Mesh.Draw(); // move paricles into single vertexbuffer
    }

    // Draw Particles
    for (size_t i = 0; i < ARRAYSIZE(emitters); ++i)
    {
        for (uint16_t j = 0; j < emitters[i].indices.size(); ++j) // draw each particle that is active
        {
            int16_t index = emitters[i].indices[j];
            cb.mWorld = XMMatrixTranspose(sharedPool[index].Mesh.GetWorldMatrix());

            XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
            // send to Card
            hr = immediateContext->Map((ID3D11Resource*)default_VS.GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
            memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
            immediateContext->Unmap((ID3D11Resource*)default_VS.GetConstantBuffer(), 0);
            sharedPool[index].Mesh.Draw(); // move paricles into single vertexbuffer
        }
    }


    // change 1 to 0 vsync
    bool vysnc = true;
    swapChain->Present(vysnc, 0);
}

void Update()
{
    gTimer.GetElapsedMilliseconds(); // causes view matrix to swap rows
    gTimer.Restart();
    float dt = ((float)gTimer.deltaTime / 1000.0f);

    CatchInput();

    if (!DrawGrid)
        end::MakeColorGrid(20.0f, 24, dt * 0.5f); // creates grid that changes color overtime

    // every 0.02 secs activate a particle
    float t = (std::ceilf(dt / 0.01f));
    if (t == 2.0f)
    {
        sortEmitter.indices.alloc();
        sortPool.alloc();
    }
    for (uint16_t i = 0; i < sortEmitter.indices.size(); ++i) // for every active particle, update it
    {
        sortPool[i].Velocity += sortPool[i].Gravity * dt; // apply gravity
        sortPool[i].Mesh.UpdatePosition(sortPool[i].Velocity * dt); // move particle
        sortPool[i].Lifetime -= dt; // kill it, but slowly

        if (sortPool[i].Lifetime <= 0.0f) // if particle is dead
        {
            sortPool.free(i); // free it
            sortEmitter.indices.free(i);
            
            int16_t index = (uint16_t)sortPool.size();

            sortPool[index].Mesh.SetPosition(sortEmitter.GetSpawnPositionVector());
            sortPool[index].Velocity = { RAND_FLT(-3.0f, 3.0f), 15.0f, RAND_FLT(-3.0f, 3.0f), 0.0f };
            sortPool[index].Lifetime = 3.0f;
        }
    }

    static uint16_t aI[4] = { 0, }; // this is pretty jank
    for (size_t emt = 0; emt < ARRAYSIZE(emitters); ++emt)
    {
        if (t == 2.0f)
            emitters[emt].indices.alloc();

        for (; aI[emt] < emitters[emt].indices.size(); ++aI[emt])
        {
            if (aI[emt] >= emitters[emt].indices.capacity())
                aI[emt] = 0;

            int16_t index = sharedPool.alloc(); // alloc particle so is it ready for use
            if (index == -1)
                break;
            emitters[emt].indices[aI[emt]] = index; // store the indices of the ready particles
        }
        for (uint16_t i = 0; i < emitters[emt].indices.size(); ++i) // for every active particle, update it
        {
            int16_t index = emitters[emt].indices[i];
            sharedPool[index].Velocity += sharedPool[index].Gravity * dt; // apply gravity
            sharedPool[index].Mesh.UpdatePosition(sharedPool[index].Velocity * dt); // move particle
            sharedPool[index].Lifetime -= dt; // kill it, but slowly

            if (sharedPool[index].Lifetime <= 0.0f) // if particle is dead
            {
                sharedPool.free(index); // deallocate
                emitters[emt].indices.free(i);

                sharedPool[index].Mesh.SetPosition(emitters[emt].GetSpawnPositionVector());
                sharedPool[index].Velocity = { RAND_FLT(-3.0f, 3.0f), 15.0f, RAND_FLT(-3.0f, 3.0f), 0.0f };
                sharedPool[index].Lifetime = 3.0f;
            }
        }
    }
}
