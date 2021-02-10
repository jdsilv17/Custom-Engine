
#include "ConstantBuffers.hlsli"

//#pragma pack_matrix(row_major)

// rule of 3 

PS_DEFAULT_INPUT main(VS_DEFAULT_INPUT input)
{
    PS_DEFAULT_INPUT output = (PS_DEFAULT_INPUT) 0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    output.UV = input.UV;
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), World)).xyz;
    
    output.Pos = mul(output.Pos, World);
    output.wPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}