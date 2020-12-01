// JustinGXEngine.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "JustinGXEngine.h"

#include "VertexShader.h"
#include "PixelShader.h"
#include "MeshVertexShader.h"

#include "./Assets/headers/StoneHenge.h"

#include <d3d11.h>
#include <directxmath.h>
#include <directxcolors.h>
#pragma comment(lib, "d3d11.lib")

using namespace DirectX;

// for init
ID3D11Device* myDevice;
IDXGISwapChain* swapChain;
ID3D11DeviceContext* immediateContext;

// for drawing
ID3D11RenderTargetView* RTV;
D3D11_VIEWPORT vPort;
float aspectRatio = 1;

struct ConstantBuffer
{
    XMMATRIX mWorld;
    XMMATRIX mView;
    XMMATRIX mProjection;
    //XMFLOAT4 vLightDir[2];
    //XMFLOAT4 vLightColor[2];
    //XMFLOAT4 vOutputColor;
};

// storage value for math
struct WVP
{
    XMFLOAT4X4 sWorld;
    XMFLOAT4X4 sView;
    XMFLOAT4X4 sProjection;
};

struct VERTEX_4
{
    XMFLOAT4 pos;
    XMFLOAT4 color;
    //XMFLOAT4 norm;
    //XMFLOAT2 uv;
};

UINT numOfElements = 0;
UINT numOfVerts = 0;

// Shader variables
ID3D11Buffer* vertexBuffer;
ID3D11InputLayout* vertexLayout;
ID3D11VertexShader* vShader;
ID3D11PixelShader* pShader;

// mash data
ID3D11Buffer* vertexBufferMesh;
ID3D11Buffer* indexBufferMesh;
ID3D11InputLayout* vertexMeshLayout;
ID3D11VertexShader* vMeshShader;

// z buffer
ID3D11Texture2D* zBuffer;
ID3D11DepthStencilView* zBufferView;

// Constant
ID3D11Buffer* constantBuffer;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JUSTINGXENGINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
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

        // rendering here (create function)
        immediateContext->ClearRenderTargetView(RTV, Colors::Aqua);
        immediateContext->ClearDepthStencilView(zBufferView, D3D11_CLEAR_DEPTH, 1, 0);

        // setup pipeline
        // output merger
        ID3D11RenderTargetView* tempRTV[] = { RTV };
        immediateContext->OMSetRenderTargets(1, tempRTV, zBufferView);
        // rasterizer
        immediateContext->RSSetViewports(1, &vPort);
        // input assembler
        immediateContext->IASetInputLayout(vertexLayout);

        ID3D11Buffer* tempVB[] = { vertexBuffer };
        UINT strides[] = { sizeof(VERTEX_4) };    // distance b/w 2 verts
        UINT offsets[] = { 0 };     // where to start from in the array
        immediateContext->IASetVertexBuffers(0, 1, tempVB, strides, offsets);
        immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        // Vertex shader stage
        immediateContext->VSSetShader(vShader, 0, 0);
        // Pixel shader stage
        immediateContext->PSSetShader(pShader, 0, 0);

        // draw
        //immediateContext->Draw(numOfVerts, 0);

        // make triangle 3D _check
            // turn into a pyramid _check
            // day 4 make world, view, & projection matrix

        //static float rot = 0; rot += 0.01f;
        //XMMATRIX temp = XMMatrixRotationY(rot);
        XMMATRIX temp = XMMatrixTranslation(0, 5.0f, -15.0f);
        ConstantBuffer cb = {};
        cb.mWorld = XMMatrixTranspose(
            XMMatrixMultiply(
                XMMatrixRotationY(45.0f * (XM_PI/180.0f)), temp));
        cb.mView = XMMatrixTranspose(
            XMMatrixLookAtLH({ 0, 7.0f, -20.0f }, { 0, 0, 0 }, { 0, 1.0f, 0 }));
        cb.mProjection = XMMatrixTranspose(
            XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.01f, 1000.0f));
        WVP wvp = {};
        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        XMStoreFloat4x4(&wvp.sView, cb.mView);
        XMStoreFloat4x4(&wvp.sProjection, cb.mProjection);

            // upload matrices to video card
                // Create and update a constant buffer (move variables from C++ to shaders)
        D3D11_MAPPED_SUBRESOURCE gpuBuffer;
        HRESULT hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap(constantBuffer, 0);


            // Apply matrix math in vertex shader _check
            // connect constant buffer to pipeline
        ID3D11Buffer* constants[] = { constantBuffer };
        immediateContext->VSSetConstantBuffers(0, 1, constants);

        // draw
        immediateContext->Draw(numOfVerts, 0);

        // get more complex pre-made mesh (FBX, OBJ, custom header) _check
        // load it onto the card (vertex buffer, index buffer, 
        // makes sure our shaders can process it
        // place it somewhere else in the environment

        // set pipeline
        ID3D11Buffer* meshVB[] = { vertexBufferMesh };
        UINT mesh_strides[] = { sizeof(_OBJ_VERT_) };    // distance b/w 2 verts
        UINT mesh_offsets[] = { 0 };     // where to start from in the array
        immediateContext->IASetVertexBuffers(0, 1, meshVB, mesh_strides, mesh_offsets);
        immediateContext->IASetIndexBuffer(indexBufferMesh, DXGI_FORMAT_R32_UINT, 0);
        immediateContext->IASetInputLayout(vertexMeshLayout);
        immediateContext->VSSetShader(vMeshShader, 0, 0);

        // modify world matrix b4 drawing next object
        cb.mWorld = XMMatrixIdentity();
        XMStoreFloat4x4(&wvp.sWorld, cb.mWorld);
        // send to Card
        hr = immediateContext->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
        memcpy(gpuBuffer.pData, &wvp, sizeof(WVP));
        immediateContext->Unmap(constantBuffer, 0);

        // draw it
        immediateContext->DrawIndexed(2532, 0, 0);
        // cahnge 1 to 0
        swapChain->Present(1, 0);
    }

    // release all our D3D11 interfaces
    RTV->Release();
    vertexBuffer->Release();
    vertexLayout->Release();
    vertexBufferMesh->Release();
    indexBufferMesh->Release();
    vertexMeshLayout->Release();
    vMeshShader->Release();
    constantBuffer->Release();
    vShader->Release();
    pShader->Release();
    myDevice->Release();
    swapChain->Release();
    immediateContext->Release();


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
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
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
    HRESULT hr = S_OK;
    hInst = hInstance; // Store instance handle in our global variable

    // find resize stuff
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

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

    hr = D3D11CreateDeviceAndSwapChain(  NULL, driverType, NULL, createDeviceFlags, &featureLevel, 1, D3D11_SDK_VERSION, &sd,
                                    &swapChain, &myDevice, 0, &immediateContext);

    ID3D11Resource* backbuffer;
    hr = swapChain->GetBuffer(0, __uuidof(backbuffer), reinterpret_cast<void**>(&backbuffer));

    hr = myDevice->CreateRenderTargetView(backbuffer, NULL, &RTV);

    backbuffer->Release();

    vPort.Width = width;
    vPort.Height = height;
    vPort.TopLeftX = vPort.TopLeftY = 0;
    vPort.MinDepth = 0;
    vPort.MaxDepth = 1;

    VERTEX_4 triangle[] =
    {
        // FRONT
        { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT4(0.25f, -0.25f, -0.25f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-0.25f, -0.25f, -0.25f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        // RIGHT FACE
        { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(0.25f, -0.25f, -0.25f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        // BACK FACE
        { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
        // LEFT FACE
        { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-0.25f, -0.25f, -0.25f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
        { XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) },
    };
    numOfVerts = ARRAYSIZE(triangle);

    // Create vertex buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.ByteWidth = sizeof(VERTEX_4) * numOfVerts;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData;
    ZeroMemory(&subData, sizeof(subData));
    subData.pSysMem = triangle;

    hr = myDevice->CreateBuffer(&bd, &subData, &vertexBuffer);
    if (FAILED(hr))
        return hr;

    // write, compile & load our shaders
    hr = myDevice->CreateVertexShader(VertexShader, sizeof(VertexShader), nullptr, &vShader);
    hr = myDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &pShader);

    // describe it to D3D11
    // create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    numOfElements = ARRAYSIZE(layout);

    hr = myDevice->CreateInputLayout(layout, numOfElements, VertexShader, sizeof(VertexShader), &vertexLayout);

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

    // load complex mesh on card
    // vertex buffer
    bd.ByteWidth = sizeof(StoneHenge_data);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    subData.pSysMem = StoneHenge_data;

    // vertex buffer
    hr = myDevice->CreateBuffer(&bd, &subData, &vertexBufferMesh);
    if (FAILED(hr))
        return hr;

    // index buffer
    bd.ByteWidth = sizeof(StoneHenge_indicies);
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

    subData.pSysMem = StoneHenge_indicies;

    hr = myDevice->CreateBuffer(&bd, &subData, &indexBufferMesh);
    if (FAILED(hr))
        return hr;

    // load mesh shader
    hr = myDevice->CreateVertexShader(MeshVertexShader, sizeof(MeshVertexShader), nullptr, &vMeshShader);

    D3D11_INPUT_ELEMENT_DESC meshLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    numOfElements = ARRAYSIZE(meshLayout);

    hr = myDevice->CreateInputLayout(meshLayout, numOfElements, MeshVertexShader, sizeof(MeshVertexShader), &vertexMeshLayout);

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

    hr = myDevice->CreateDepthStencilView(zBuffer, nullptr, &zBufferView);

    return TRUE;
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
