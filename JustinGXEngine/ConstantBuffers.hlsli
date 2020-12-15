cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4 LightPos[3];
    float4 LightDir[3];
    float4 LightColor[3];
    float4 camPos;
    float4 OutputColor;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_SKY_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 UVW : TEXCOORD;
};

struct VS_DEFAULT_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_SOLID_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

struct PS_SKY_INPUT
{
    float4 Pos : SV_POSITION;
    float3 UVW : TEXCOORD;
    float3 LPos : POSTION;
};