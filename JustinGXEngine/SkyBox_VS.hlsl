#include "ConstantBuffers.hlsli"

PS_SKY_INPUT main(VS_SKY_INPUT input)
{
    PS_SKY_INPUT output = (PS_SKY_INPUT) 0;
    output.LPos = input.Pos.xyz; // Local position
    
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection); // set to xyww so z will always be 1 (furthest from camera in z-buffer)
    
    output.UVW = input.UVW;
    
    return output;
}