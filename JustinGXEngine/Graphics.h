#pragma once

#include "Cube.h"
#include "Camera.h"
#include "Light.h"
#include "Shaders.h"
#include "Animation.h"
#include "BinaryFileLoad.h"

#include "Particle.h"
#include "Emitter.h"
#include "pools.h"
#include "debug_renderer.h"
#include "frustum_culling.h" // solved circular dependency problem by making functions inline
#include "bvh.h"

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <directxcolors.h>
#include <iostream>
#include <bitset> // for std::bitset
#include <algorithm> // for std::shuffle, Dont need this either
#include <random> // for std::mt19937 g(rand()); Dont think I need this anymore
#pragma comment(lib, "d3d11.lib")

//struct ConstantBuffer
//{
//    DirectX::XMMATRIX mWorld;
//    DirectX::XMMATRIX mView;
//    DirectX::XMMATRIX mProjection;
//    DirectX::XMFLOAT4 LightPos[3];
//    DirectX::XMFLOAT4 LightDir[3];
//    DirectX::XMFLOAT4 LightColor[3];
//};

//storage value for math
struct WVP
{
    DirectX::XMFLOAT4X4 sWorld;
    DirectX::XMFLOAT4X4 sView;
    DirectX::XMFLOAT4X4 sProjection;
    DirectX::XMFLOAT4X4 SkinMat[28];
    //DirectX::XMFLOAT4X4 SkinNoramlMat[28];
    DirectX::XMFLOAT4 LightPos[3];
    DirectX::XMFLOAT4 LightDir[3];
    DirectX::XMFLOAT4 LightColor[3];
    DirectX::XMFLOAT4 CamPos;
    DirectX::XMFLOAT4 totalTime;
};

struct material_t
{
    enum e_component { EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT };

    struct component_t
    {
        float value[3] = { 0.0f, 0.0f, 0.0f };
        float factor = 0.0f;
        int64_t input = -1;
    };

    component_t& operator[](int i) { return components[i]; }
    const component_t& operator[](int i)const { return components[i]; }

private:
    component_t components[COUNT];
};


//class Cube;
//class Camera;
//class DirectionalLight;
//class PointLight;
//class SpotLight;

#define RAND_FLT(min, max)  (min + (rand() / (float)RAND_MAX) * (max - min))

class Graphics
{
public:
    bool Initialize(HWND hWnd/*, int width, int height*/);
    void RenderFrame();
    void CleanUp();

private:
    bool InitDirectX(HWND hWnd);
    bool InitShaders();
    bool InitScene(/*Scene scene*/);

    // for init
    ID3D11Device* myDevice = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11DeviceContext* immediateContext = nullptr;

    // for drawing
    ID3D11RenderTargetView* RTV = nullptr;
    D3D11_VIEWPORT vPort;
    float aspectRatio = 1;

    UINT numOfElements = 0;
    UINT numOfVerts = 0;

    // z buffer
    ID3D11Texture2D* zBuffer = nullptr;
    ID3D11DepthStencilView* zBufferView = nullptr;

    ID3D11DepthStencilState* DSLessEqual = nullptr; // used to make sure skybox is always behind all the other geometry
    ID3D11DepthStencilState* DSNoDepth = nullptr; // turns depth off;
    ID3D11RasterizerState* RSCullNone = nullptr; // used to disable backface-culling
    ID3D11RasterizerState* RSAALLines = nullptr; // used to enable anti-aliased lines

    ID3D11BlendState* blendState = nullptr;

#ifdef _DEBUG
    ID3D11Debug* debug = nullptr;
#endif

public: // temp
    DirectX::XMVECTOR LightPositions[3] =
    {
        {-0.577f, 0.577f, -0.577f, 1.0f}, // directional
        {-10.0f, 5.0f, 0.0f, 1.0f}, // point
        {5.0f, 5.0f, 0.0f, 1.0f} // spot
    };
    DirectX::XMVECTOR LightDirs[3] =
    {
        {-0.577f, 0.577f, -0.577f, 1.0f}, // directional
        {-10.0f, 5.0f, 0.0f, 1.0f}, // point
        {1.0f, -1.0f, 0.0f, 1.0f} // spot
    };
    DirectX::XMVECTOR LightColors[3] =
    {
        {0.75f, 0.75f, 0.75f, 1.0f},
        {1.0f, 0.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 0.0f, 1.0f}
    };
    /*************** Objects ***************/
    //Time gTimer;
    Camera cam;
    Mesh<VERTEX> grid;
    //Mesh<_OBJ_VERT_> DwarfBody;
    //Mesh<_OBJ_VERT_> DwarfShirt;
    //Mesh<_OBJ_VERT_> DwarfLeather;
    //Mesh<_OBJ_VERT_> DwarfArmor;
    //Mesh<_OBJ_VERT_> DwarfAxe;
    //Mesh<VERTEX> HUD;

    //Mesh<_OBJ_VERT_> planet_1;
    //Mesh<_OBJ_VERT_> planet_2;
    //Mesh<_OBJ_VERT_> planet_3;
    //Mesh<_OBJ_VERT_> moon;
    //Mesh<_OBJ_VERT_> talon;
    Mesh<VERTEX> point;
    Cube skybox;
    DirectionalLight dirLight;
    PointLight pntLight;
    SpotLight sptLight;

    // engine dev
    Emitter sortEmitter;
    end::Sorted_Pool_t<Particle, 256> sortPool;
    Emitter emitters[4];
    end::Pool_t<Particle, 1024> sharedPool;
    std::vector<Object> Gizmos;
    end::aabb_t aabbs[3];
    std::bitset<256> bits;
    //std::vector<XMFLOAT3> terrain_pos;
    //std::vector<VERTEX> terrain_verts;
    //std::vector<VERTEX*> terrain_triangles;
    //std::vector<size_t> terrain_tri_indices;
    //std::vector<DirectX::XMFLOAT4> terrain_centroids;
    //std::vector<end::bvh_node_t> BVH;

    Mesh<VERTEX_ANIM> BattleMage;
    Animation::Animation run_anim;


    Shaders::VertexShader advanced_VS;
    Shaders::VertexShader default_VS;
    Shaders::VertexShader skybox_VS;
    Shaders::VertexShader gs_VS;
    Shaders::VertexShader anim_VS;
    Shaders::PixelShader advanced_PS;
    Shaders::PixelShader solid_PS;
    Shaders::PixelShader skybox_PS;
    Shaders::PixelShader default_PS;
    Shaders::PixelShader anim_PS;

    //Shaders::VertexShader HUD_VS;
    //Shaders::VertexShader Smoke_VS;
    //Shaders::PixelShader DwarfBody_PS;
    //Shaders::PixelShader DwarfShirt_PS;
    //Shaders::PixelShader DwarfLeather_PS;
    //Shaders::PixelShader DwarfArmor_PS;
    //Shaders::PixelShader DwarfAxe_PS;
    //Shaders::PixelShader HUD_PS;
    //Shaders::PixelShader Smoke_PS;

    //Shaders::PixelShader planet1_PS;
    //Shaders::PixelShader planet2_PS;
    //Shaders::PixelShader planet3_PS;
    //Shaders::PixelShader moon_PS;
    //Shaders::PixelShader talon_PS;
    Shaders::GeometryShader pntToQuad_GS;
    Shaders::GeometryShader viewportTEST_GS;

    bool DrawQuad = false;
    bool DrawGrid = false;
};

