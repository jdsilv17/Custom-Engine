#include "ConstantBuffers.hlsli"

struct GSOutput
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    uint viewport : SV_ViewportArrayIndex;
};

[maxvertexcount(2)]
void main(
	line PS_DEFAULT_INPUT input[2], 
	inout LineStream< GSOutput > output
)
{
    GSOutput element;
    element.viewport = 1; // second viewport
    
    for (uint i = 0; i < 2; ++i)
	{
		element.Pos = input[i].Pos;
        element.wPos = input[i].wPos;
        element.Color = input[i].Color;
        element.Normal = input[i].Normal;
        element.UV = input[i].UV;
		
		output.Append(element);
	}
}