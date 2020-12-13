#include "ConstantBuffers.hlsli"

PS_SKY_INPUT main(VS_DEFAULT_INPUT input)
{
    PS_SKY_INPUT output = (PS_SKY_INPUT) 0;
    
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection).xyww; // set to xyww so z will always be 1 (furthest from camera in z-buffer)
    
    output.UVW = input.Pos.xyz;
    
    return output;
}