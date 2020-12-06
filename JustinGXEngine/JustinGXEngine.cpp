// JustinGXEngine.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "JustinGXEngine.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "MeshVertexShader.h"

#ifndef MeshUtils
    #include "MeshUtils.h"
    #define MeshUtils
#endif // MeshUtils

#include "Mesh.h"
#include "Camera.h"

#include "./Assets/headers/StoneHenge.h"
#include "./Assets/headers/test pyramid.h"

#include "DDSTextureLoader.h"
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <fstream>
#pragma comment(lib, "d3d11.lib")

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
    XMFLOAT4 LightDir[2];
    XMFLOAT4 LightColor[2];
    XMFLOAT4 OutputColor;
};

 //storage value for math
struct WVP
{
    XMFLOAT4X4 sWorld;
    XMFLOAT4X4 sView;
    XMFLOAT4X4 sProjection;
    XMFLOAT4 LightDir[2];
    XMFLOAT4 LightColor[2];
    XMFLOAT4 OutputColor;
};

XMVECTOR LightDirs[2] =
{
    {-0.577f, 0.577f, -0.577f, 1.0f},
    { 0.577f, 0.2577f, -0.577f, 1.0f }
    //{0.0f, 2.0f, -1.0f, 1.0f},
};
XMVECTOR LightColors[2] =
{
    {0.75f, 0.75f, 0.75f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f}
};


//Mesh<VERTEX_4> cube;

UINT numOfElements = 0;
UINT numOfVerts = 0;

// Shader variables
ID3D11Buffer* vertexBuffer = nullptr;
ID3D11Buffer* indexBuffer = nullptr;
ID3D11InputLayout* vertexLayout = nullptr;
ID3D11VertexShader* vShader = nullptr;
ID3D11PixelShader* pShader = nullptr;
// Texturing
ID3D11SamplerState* samplerState = nullptr;
//ID3D11Texture2D* tex_Stonehenge;
ID3D11ShaderResourceView* SRV = nullptr;
ID3D11ShaderResourceView* SRV_2 = nullptr;

// mash data
ID3D11Buffer* vertexBufferMesh = nullptr;
ID3D11Buffer* indexBufferMesh = nullptr;
ID3D11InputLayout* vertexMeshLayout = nullptr;
ID3D11VertexShader* vMeshShader = nullptr;

// z buffer
ID3D11Texture2D* zBuffer = nullptr;
ID3D11DepthStencilView* zBufferView = nullptr;

// Constant
ID3D11Buffer* constantBuffer = nullptr;
#ifdef _DEBUG
ID3D11Debug* debug = nullptr;
#endif


Camera cam;


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
void                ExecutePipeline();

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

    // Main message loop:
    while (msg.message != WM_QUIT) //GetMessage(&msg, nullptr, 0, 0)) waits for message
    {
        PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);

        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        ExecutePipeline();
    }

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
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
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

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif


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

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // Create the depth stencil view
    //D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    //descDSV.Format = descDepth.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    //descDSV.Texture2D.MipSlice = 0;
    hr = myDevice->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // Setup viewport
    vPort.Width = static_cast<FLOAT>(width);
    vPort.Height = static_cast<FLOAT>(height);
    vPort.TopLeftX = vPort.TopLeftY = 0;
    vPort.MinDepth = 0;
    vPort.MaxDepth = 1;

    

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    return hr;
}

HRESULT InitContent()
{
    HRESULT hr = S_OK;
    
    //cube.VertexList =
    //{
    //    //// TOP
    //    //{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(-1.0f, 0.0f) },  // back left
    //    //{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },    // back right
    //    //{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },     // front right
    //    //{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT2(-1.0f, 1.0f) },  // front left
    //    //// BOTTOM
    //    //{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) }, // back left
    //    //{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(-1.0f, 0.0f) }, // back right
    //    //{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(-1.0f, 1.0f) },  // front right
    //    //{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },  // front left
    //    //// LEFT SIDE
    //    //{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },  // front bottom
    //    //{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(-1.0f, 1.0f) },// 
    //    //{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(-1.0f, 0.0f) },
    //    //{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    //    //// RIGHT SIDE
    //    //{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(-1.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
    //    //{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(-1.0f, 0.0f)  },
    //    //// BACK
    //    //{ XMFLOAT4(-1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, -1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(-1.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(-1.0f, 0.0f) },
    //    //{ XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
    //    //// FRONT
    //    //{ XMFLOAT4(-1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(-1.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, -1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
    //    //{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
    //    //{ XMFLOAT4(-1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(-1.0f, 0.0f) },

    //    //    // front face
    //    //{{-0.25f, 0.25f, -0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, //0 top left
    //    //{{0.25f, 0.25f, -0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, //1 top right
    //    //{{0.25f, -0.25f, -0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, //2 bottom right
    //    //{{-0.25f, -0.25f, -0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, //3 bottom left
    //    ////// back face
    //    //{{-0.25f, 0.25f, 0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}, //4 back top left
    //    //{{0.25f, 0.25f, 0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, //5 back top right
    //    //{{0.25f, -0.25f, 0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, //6 back bottom right
    //    //{{-0.25f, -0.25f, 0.25f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, //7 back bottom left
    //};
    //numOfVerts = cube.VertexList.size();

    // Create vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    //bd.ByteWidth = sizeof(VERTEX_4) * numOfVerts;
    //bd.Usage = D3D11_USAGE_DEFAULT;
    //bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //bd.CPUAccessFlags = 0;
    //bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    //subData.pSysMem = &cube.VertexList;

    //hr = myDevice->CreateBuffer( &bd, &subData, cube.VertexBuffer.ReleaseAndGetAddressOf() );
    //if (FAILED(hr))
    //    return hr;

    // write, compile & load our shaders
    hr = myDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);    

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    hr = myDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // describe it to D3D11
    // create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    numOfElements = ARRAYSIZE(layout);

    //hr = myDevice->CreateInputLayout(layout, numOfElements, VertexShader, sizeof(VertexShader), cube.InputLayout.ReleaseAndGetAddressOf());

    // create constant buffer
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(WVP);
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;

    hr = myDevice->CreateBuffer(&bd, nullptr, &constantBuffer);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

//    #pragma region CUBE INDEX BUFFER
//    cube.IndicesList =
//    {
//        0,1,3,
//        2,1,3,
//
//        6,4,5,
//        7,4,6,
//
//        11,9,8,
//        10,9,11,
//
//        14,12,13,
//        15,12,14,
//
//        19,17,16,
//        18,17,19,
//
//        22,20,21,
//        23,20,22
//    };
//
//    // index buffer
//    //bd.ByteWidth = sizeof(int) * cube.IndicesList.size();
//    //bd.Usage = D3D11_USAGE_IMMUTABLE;
//    //bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    //bd.CPUAccessFlags = 0;
//    //bd.MiscFlags = 0;
//
//    //subData.pSysMem = &cube.IndicesList;
//
//    //hr = myDevice->CreateBuffer(&bd, &subData, cube.IndexBuffer.ReleaseAndGetAddressOf());
//    //if (FAILED(hr))
//    //    return hr;
//#pragma endregion

    // load complex mesh on card=================================================================================
    // vertex buffer
    bd.ByteWidth = sizeof(test_pyramid_data);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    subData.pSysMem = test_pyramid_data;

    // Create vertex buffer
    hr = myDevice->CreateBuffer(&bd, &subData, &vertexBufferMesh);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    bd.ByteWidth = sizeof(StoneHenge_data);
    subData.pSysMem = StoneHenge_data;
    hr = myDevice->CreateBuffer(&bd, &subData, &vertexBuffer);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // index buffer
    bd.ByteWidth = sizeof(test_pyramid_indicies);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    subData.pSysMem = test_pyramid_indicies;

    hr = myDevice->CreateBuffer(&bd, &subData, &indexBufferMesh);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    bd.ByteWidth = sizeof(StoneHenge_indicies);
    subData.pSysMem = StoneHenge_indicies;
    hr = myDevice->CreateBuffer(&bd, &subData, &indexBuffer);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // load mesh shader
    hr = myDevice->CreateVertexShader(MeshVertexShader, sizeof(MeshVertexShader), nullptr, &vMeshShader);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    D3D11_INPUT_ELEMENT_DESC meshLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    numOfElements = ARRAYSIZE(meshLayout);

    hr = myDevice->CreateInputLayout(meshLayout, numOfElements, MeshVertexShader, sizeof(MeshVertexShader), &vertexMeshLayout);
    if (FAILED(hr))
        return hr;

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // TEXTURING ===============================================================================================
    // Create Texture for Stonehenge
    //myDevice->CreateShaderResourceView()
    std::string filename = "./Assets/Textures/StoneHenge.dds";
    std::wstring widestr = std::wstring(filename.begin(), filename.end());
    const wchar_t* widecstr = widestr.c_str();
    hr = CreateDDSTextureFromFile(myDevice, widecstr, nullptr, &SRV);

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    filename = "./Assets/Textures/fire_01.dds";
    widestr = std::wstring(filename.begin(), filename.end());
    widecstr = widestr.c_str();
    hr = CreateDDSTextureFromFile(myDevice, widecstr, nullptr, &SRV_2);

#ifdef _DEBUG
    hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
#endif

    // Create Default Sampler State
    D3D11_SAMPLER_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sd.MinLOD = 0;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    hr = myDevice->CreateSamplerState(&sd, &samplerState);
    if (FAILED(hr))
        return hr;

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
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexLayout) vertexLayout->Release();
    if (vertexBufferMesh) vertexBufferMesh->Release();
    if (indexBufferMesh) indexBufferMesh->Release();
    if (vertexMeshLayout) vertexMeshLayout->Release();
    if (vMeshShader) vMeshShader->Release();
    if (zBuffer) zBuffer->Release();
    if (zBufferView) zBufferView->Release();
    if (constantBuffer) constantBuffer->Release();
    if (SRV) SRV->Release();
    if (SRV_2) SRV_2->Release();
    if (samplerState) samplerState->Release();
    if (vShader) vShader->Release();
    if (pShader) pShader->Release();
    if (swapChain) swapChain->Release();
    if (immediateContext) immediateContext->Release();
    if (myDevice) myDevice->Release();
}

/// <summary>
/// Catches the keyboard and mouse input
/// </summary>
void CatchInput()
{
    const float cameraSpeed = 0.2f;
    if (GetAsyncKeyState('W') & 0x8000)
    {
        cam.UpdatePosition(cam.GetForwardVector() * cameraSpeed);
    }
    if (GetAsyncKeyState('A') & 0x8000)
    {
        cam.UpdatePosition(cam.GetLeftVector() * cameraSpeed);
    }
    if (GetAsyncKeyState('S') & 0x8000)
    {
        cam.UpdatePosition(cam.GetBackwardVector() * cameraSpeed);
    }
    if (GetAsyncKeyState('D') & 0x8000)
    {
        cam.UpdatePosition(cam.GetRightVector() * cameraSpeed);
    }
    if (GetAsyncKeyState(VK_SPACE) & 0x8000)
    {
        cam.UpdatePosition(cam.GetUpVector() * cameraSpeed);
    }
    if (GetAsyncKeyState('X') & 0x8000)
    {
        cam.UpdatePosition(cam.GetUpVector() * -cameraSpeed);
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
    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: Add any drawing code that uses hdc here...
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;
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

void ExecutePipeline()
{
    // rendering here (create function)
    immediateContext->ClearRenderTargetView(RTV, Colors::DarkBlue);
    immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

    // setup pipeline
        // IA (Input Assembler)
        // VS (Vertex Shader)
        // RS (Rasterizer Stage)
        // PS (Pixel Shader)
        // OM (Output Merger)

    // output merger
    ID3D11RenderTargetView* tempRTV[] = { RTV };
    immediateContext->OMSetRenderTargets(1, tempRTV, zBufferView);
    // rasterizer
    immediateContext->RSSetViewports(1, &vPort);
    // input assembler
    //immediateContext->IASetInputLayout(cube.InputLayout.Get());

    //ID3D11Buffer* tempVB[] = { cube.VertexBuffer.GetAddressOf() };
    UINT strides[] = { sizeof(VERTEX_4) };    // distance b/w 2 verts
    UINT offsets[] = { 0 };     // where to start from in the array
    //immediateContext->IASetVertexBuffers(0, 1, cube.VertexBuffer.GetAddressOf()/*tempVB*/, strides, offsets);
    //immediateContext->IASetIndexBuffer(cube.IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // Vertex shader stage
    //immediateContext->VSSetShader(vShader, 0, 0);
    // Pixel shader stage
    ID3D11ShaderResourceView* tempSRV[] =
    {
        SRV,
        SRV_2
    };
    immediateContext->PSSetShaderResources(0, 2, tempSRV);
    immediateContext->PSSetSamplers(0, 1, &samplerState);
    immediateContext->PSSetShader(pShader, 0, 0);

    // make triangle 3D _check
        // turn into a pyramid _check
        // day 4 make world, view, & projection matrix
    CatchInput();
    
    static float rot = 0; rot += 0.01f;
    XMMATRIX temp = XMMatrixRotationY(rot);
    XMMATRIX temp2 = XMMatrixTranslation(1.5f, 0, 0);
    ConstantBuffer cb = {};
    //cb.mWorld = XMMatrixTranspose(
    //    XMMatrixMultiply(
    //        XMMatrixRotationY(45.0f * (XM_PI / 180.0f)), temp2));
    cb.mView = XMMatrixTranspose( /*XMMatrixMultiply( temp,*/ cam.GetViewMatrix() );
    //cam.SetProjectionMatrix(XM_PIDIV4, aspectRatio, 0.01f, 1000.0f);
    cam.SetProjectionMatrix(45.0f, aspectRatio, 0.1f, 1000.0f);
    cb.mProjection = XMMatrixTranspose(
        cam.GetProjectionMatrix());
    WVP wvp = {};
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    XMStoreFloat4x4(&wvp.sView, cb.mView);
    XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);
    XMStoreFloat4(&wvp.LightDir[0], LightDirs[0]);
    //LightDirs[1] = XMVector4Transform(LightDirs[1], temp);
    XMStoreFloat4(&wvp.LightDir[1], LightDirs[1]);
    XMStoreFloat4(&wvp.LightColor[0], LightColors[0]);
    XMStoreFloat4(&wvp.LightColor[1], LightColors[1]);

    // upload matrices to video card
        // Create and update a constant buffer (move variables from C++ to shaders)
    D3D11_MAPPED_SUBRESOURCE gpuBuffer;
    HRESULT hr;


    // Apply matrix math in vertex shader _check
    // connect constant buffer to pipeline
    //ID3D11Buffer* constants[] = { constantBuffer };
    immediateContext->VSSetConstantBuffers(0, 1, &constantBuffer);
    immediateContext->PSSetConstantBuffers(0, 1, &constantBuffer);

    // draw
    //immediateContext->DrawIndexed(cube.IndicesList.size(), 0, 0);

    //======================================================================================================================
    // get more complex pre-made mesh (FBX, OBJ, custom header) _check
    // load it onto the card (vertex buffer, index buffer, 
    // makes sure our shaders can process it
    // place it somewhere else in the environment

    ID3D11Buffer* meshVB[] = { vertexBuffer };
    UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };    // distance b/w 2 verts
    UINT mesh_offsets[] = { 0 };     // where to start from in the array
    immediateContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
    immediateContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetInputLayout(vertexMeshLayout);
    immediateContext->VSSetShader(vMeshShader, 0, 0);
    immediateContext->PSSetSamplers(0, 1, &samplerState);
    // modify world matrix b4 drawing next object
    cb.mWorld = XMMatrixTranspose(
        XMMatrixTranslation(0, 0, 0));
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap(constantBuffer, 0);

    // draw it
    immediateContext->DrawIndexed(2532, 0, 0);

    // set pipeline
    ID3D11Buffer* VB[] = { vertexBufferMesh };

    immediateContext->IASetVertexBuffers(0, 1, VB, mesh_strides, mesh_offsets);
    immediateContext->IASetIndexBuffer(indexBufferMesh, DXGI_FORMAT_R32_UINT, 0);
    immediateContext->IASetInputLayout(vertexMeshLayout);
    immediateContext->VSSetShader(vMeshShader, 0, 0);

    // Draw pyramid
    // modify world matrix b4 drawing next object
    cb.mWorld = XMMatrixTranspose(
        XMMatrixMultiply(
            temp, XMMatrixTranslation(0, 6.0f, 0)));
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap(constantBuffer, 0);

    // draw it
    immediateContext->DrawIndexed(1674, 0, 0);

    // Draw Directional Light
    // modify world matrix b4 drawing next object
    cb.mWorld = XMMatrixTranspose( XMMatrixTranslationFromVector( 5.0f * LightDirs[0] ) );
    XMMATRIX mLightScale = XMMatrixScaling(5.0f, 5.0f, 5.0f);
    cb.mWorld = mLightScale * cb.mWorld;
    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap(constantBuffer, 0);

    // draw it
    immediateContext->DrawIndexed(1674, 0, 0);

    // Draw Point Light Light
    // modify world matrix b4 drawing next object
    cb.mWorld = XMMatrixTranspose(
            XMMatrixTranslationFromVector( 5.0f * LightDirs[1]));

    XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
    // send to Card
    hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
    memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
    immediateContext->Unmap(constantBuffer, 0);

    // draw it
    immediateContext->DrawIndexed(1674, 0, 0);


    // cahnge 1 to 0
    swapChain->Present(1, 0);
}
