#include "ConstantBuffers.hlsli"

Texture2D tex : register(t0);
SamplerState sampleState : register(s0);

float4 main(GS_OUTPUT input) : SV_TARGET
{
    float2 UV1, UV2;
    
    UV1.x = input.UVW.x + (totalTime.x * 0.015f) + (sin(totalTime.x + input.UVW.y * 10.0f) * 0.015f);
    UV1.y = input.UVW.y;
    
    UV2.x = input.UVW.x + (totalTime.x * -0.025f);
    UV2.y = input.UVW.y * (0.7f + (sin(totalTime.x * 0.5f + input.UVW.x * 10.0f) * 0.07f));
    
    float4 color = float4(0.25f, 0.25f, 0.25f, 0.15);
    float4 tex1 = tex.Sample(sampleState, UV1);
    float4 tex2 = tex.Sample(sampleState, UV2);
    
    return tex1 * color + tex2 * color;
}