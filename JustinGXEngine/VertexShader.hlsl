
#include "ConstantBuffers.hlsli"

//#pragma pack_matrix(row_major)

// rule of 3 

PS_SOLID_INPUT main(VS_DEFAULT_INPUT input)
{
    PS_SOLID_INPUT output = (PS_SOLID_INPUT) 0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    
    output.Pos = mul(output.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}