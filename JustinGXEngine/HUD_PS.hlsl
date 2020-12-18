#include "ConstantBuffers.hlsli"

Texture2D tex : register(t0);
SamplerState sampleState : register(s0);

float4 main(GS_OUTPUT input) : SV_TARGET
{
    float4 hud = tex.Sample(sampleState, input.UVW.xy);
    return hud;
}