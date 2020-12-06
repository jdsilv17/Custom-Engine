

Texture2D textures[2] : register(t0);
SamplerState samplerState : register(s0);

cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4 LightDir[2];
    float4 LightColor[2];
    float4 OutputColor;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse);
float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse);
float CalcAttenuation(float4 lPos, float lRadius, float4 sPos);
float CalcAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio);

float4 PS_MultiTexturing(PS_INPUT input);
float4 PS_SingleTexture(PS_INPUT input);
float4 PS_Solid(PS_INPUT input);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 texColor = PS_MultiTexturing(input);
    
    float4 finalColor = CalcDirectinalLight(LightDir[0].xyz, LightColor[0], input.Normal, texColor);
    
    float pntRadius = 0.5f;
    finalColor += CalcPointLight(LightDir[1], LightColor[1], pntRadius, input.Pos.xyzw, input.Normal, finalColor);
    
    return texColor * finalColor;
}

float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse)
{
    if (length(sNormal) != 1.0f)
        sNormal = normalize(sNormal);
    if (length(lDir) != 1.0f)
        lDir = normalize(lDir);
    
    float ambientTerm = 0.7f;
    
    // directional lightratio
    float lightRatio = saturate(dot(-1.0f * lDir, sNormal) + ambientTerm);
    
    //float4 finalColor = saturate(dot(-1.0f * lDir, sNormal) * tDiffuse);
    //finalColor += tDiffuse * lColor;
    //finalColor *= texColor;
    //finalColor.a = 1.0f;
    
    // lerp from light color to black by the light ratio
    float4 finalColor = lerp(0, LightColor[0], lightRatio);
    
    return finalColor;
}

float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse)
{
    //if (length(sPos) != 1.0f)
    //    sPos = normalize(sPos);
    
    float ambientTerm = 0.5f;
    
    float4 lightDir = normalize(lPos - sPos);
    
    float lightRatio = saturate(dot(lightDir.xyz, sNormal) + ambientTerm);
    
    float attenuation = CalcAttenuation(lPos, lRadius, sPos);
    
    float4 finalColor = lerp(0, lColor, lightRatio * attenuation);
    
    return finalColor;
}

float CalcAttenuation(float4 lPos, float lRadius, float4 sPos)
{
    return pow(1.0f - saturate(length(lPos - sPos) / lRadius), 2.0f);
}

float CalcAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio)
{
    return pow( 1.0f - saturate( (innerConeRatio - surfaceRatio) / (innerConeRatio - outerConeRatio) ), 2.0f );
}

float4 PS_MultiTexturing(PS_INPUT input)
{
    
    // get first color for pixel
    float4 baseColor = textures[0].Sample(samplerState, input.UVW.xy);
    // get second color for pixel
    float4 detailColor = textures[1].Sample(samplerState, input.UVW.xy);
    // blend the pixel's color together
    //float4 blendColor = baseColor * detailColor * 2.0f;
    
    //float4 finalColor = saturate(blendColor);
    float4 finalColor = float4(lerp(baseColor.rgb, detailColor.rgb, detailColor.a), baseColor.a);
    
    clip(finalColor.a);
    
    return finalColor;
}

float4 PS_SingleTexture(PS_INPUT input)
{
    float4 baseColor = textures[0].Sample(samplerState, input.UVW.xy);
    return baseColor;
}

float4 PS_Solid(PS_INPUT input)
{
    float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    return color;
}