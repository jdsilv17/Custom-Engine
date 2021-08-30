
#include "ConstantBuffers.hlsli"

//#pragma pack_matrix(row_major)

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.Pos = float4(input.Pos, 1);
    output.UVW = input.UVW;
    
    output.Pos = mul(output.Pos, World);
    output.wPos = output.Pos;
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), World)).xyz;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}