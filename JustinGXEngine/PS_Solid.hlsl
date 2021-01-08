
#include "ConstantBuffers.hlsli"

float4 main(PS_SOLID_INPUT input) : SV_TARGET
{
    return saturate(input.Color);
}