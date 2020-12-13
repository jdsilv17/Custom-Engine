#include "ConstantBuffers.hlsli"

TextureCube cubeTexture : register(t0);
SamplerState samplerState : register(s0);

float4 main(PS_SKY_INPUT input) : SV_TARGET
{
    return cubeTexture.Sample(samplerState, input.UVW);
}