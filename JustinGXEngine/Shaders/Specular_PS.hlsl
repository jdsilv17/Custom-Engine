
#include "ConstantBuffers.hlsli"

Texture2D textures[2] : register(t0);
SamplerState samplerState : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 BaseTex = PS_SingleTexture(input.UVW.xy, textures[0], samplerState);
    float4 SpecTex = PS_SingleTexture(input.UVW.xy, textures[1], samplerState);
    float4 ambientLight = float4(0.3f, 0.3f, 0.3f, 1.0f);
    float4 finalColor = ambientLight;
    finalColor += CalcDirectinalLight(LightDir[0].xyz, LightColor[0], input.Normal, BaseTex);
    
    float pntRadius = 5.0f;
    finalColor += CalcPointLight(LightPos[1], LightColor[1], pntRadius, input.wPos, input.Normal, BaseTex, SpecTex);
    
    //finalColor += CalcSpotLight(LightPos[2], (float3) LightDir[1], LightColor[2], input.wPos, input.Normal, texColor);
    
    
    return saturate(finalColor);
}