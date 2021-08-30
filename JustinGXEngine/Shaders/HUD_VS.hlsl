#include "ConstantBuffers.hlsli"

GS_OUTPUT main(VS_DEFAULT_INPUT input)
{
    GS_OUTPUT output = (GS_OUTPUT) 0;
    output.Pos = input.Pos;
    output.Color = input.Color;
    output.UVW.xy = input.UV;
    return output;
}