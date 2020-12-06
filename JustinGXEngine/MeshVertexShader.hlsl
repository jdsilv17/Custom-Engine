
//#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4 LightDir[2];
    float4 LightColor[2];
    float4 OutputColor;
}

// rule of 3 
struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = float4(input.Pos, 1);
    output.UVW = input.UVW;
    
    output.Pos = mul(output.Pos, World);
    output.Normal = mul(input.Normal, (float3x3) World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}