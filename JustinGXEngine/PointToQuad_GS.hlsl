#include "ConstantBuffers.hlsli"

[maxvertexcount(4)]
void main(
	point VS_DEFAULT_INPUT input[1],
	inout TriangleStream<GS_OUTPUT> output
)
{
    GS_OUTPUT element[4];
    element[0].Pos = float4(input[0].Pos.x - 100.0f, input[0].Pos.y, input[0].Pos.z + 100.0f, 1.0f);
    element[1].Pos = float4(input[0].Pos.x + 100.0f, input[0].Pos.y, input[0].Pos.z + 100.0f, 1.0f);
    element[2].Pos = float4(input[0].Pos.x - 100.0f, input[0].Pos.y, input[0].Pos.z - 100.0f, 1.0f);
    element[3].Pos = float4(input[0].Pos.x + 100.0f, input[0].Pos.y, input[0].Pos.z - 100.0f, 1.0f);
    
    element[0].Color = input[0].Color;
    element[1].Color = input[0].Color;
    element[2].Color = input[0].Color;
    element[3].Color = input[0].Color;
	
    element[0].UVW = float3(0.0f, 0.0f, 0.0f);
    element[1].UVW = float3(1.0f, 0.0f, 0.0f);
    element[2].UVW = float3(1.0f, 1.0f, 0.0f);
    element[3].UVW = float3(0.0f, 1.0f, 0.0f);
    
    element[0].Normal = input[0].Normal;
    element[1].Normal = input[0].Normal;
    element[2].Normal = input[0].Normal;
    element[3].Normal = input[0].Normal;

	for (uint i = 0; i < 4; i++)
	{
        element[i].Pos = mul(element[i].Pos, View);
        element[i].Pos = mul(element[i].Pos, Projection);
		output.Append(element[i]);
	}
}