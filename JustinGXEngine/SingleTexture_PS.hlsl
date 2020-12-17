
#include "ConstantBuffers.hlsli"

Texture2D textures : register(t0);
SamplerState samplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 texColor = PS_SingleTexture(input, textures, samplerState);
    float4 finalColor = 0;
    finalColor = CalcDirectinalLight(LightDir[0].xyz, LightColor[0], input.Normal, texColor);
    
    float pntRadius = 25.0f;
    finalColor += CalcPointLight(LightPos[1], LightColor[1], pntRadius, input.wPos, input.Normal, texColor);
    
    //finalColor += CalcSpotLight(LightPos[2], (float3) LightDir[1], LightColor[2], input.wPos, input.Normal, float4(0.5, 0.5f, 0.5f, 1.0f));
    
    return finalColor;
}