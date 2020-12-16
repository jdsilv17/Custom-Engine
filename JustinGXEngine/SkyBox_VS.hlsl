#include "ConstantBuffers.hlsli"

PS_SKY_INPUT main(VS_SKY_INPUT input)
{
    PS_SKY_INPUT output = (PS_SKY_INPUT) 0;
    output.Pos = float4(input.Pos, 1.0f);
    output.LPos = input.Pos; // Local position
    
    output.Pos = mul(output.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection).xyww; // set to xyww so z will always be 1 (furthest from camera in z-buffer)
    
    output.UVW = input.UVW;
    
    return output;
}