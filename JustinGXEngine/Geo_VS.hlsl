#include "ConstantBuffers.hlsli"

//#pragma pack_matrix(row_major)

VS_DEFAULT_INPUT main(VS_DEFAULT_INPUT input)
{
    VS_DEFAULT_INPUT output = (VS_DEFAULT_INPUT) 0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    output.UV = input.UV;
    output.Normal = input.Normal;
    
    output.Pos = mul(output.Pos, World);
    
    //output.Normal = normalize(mul(float4(input.Normal, 0.0f), World)).xyz;
    //output.Pos = mul(output.Pos, View);
    //output.Pos = mul(output.Pos, Projection);
    
    return output;
}