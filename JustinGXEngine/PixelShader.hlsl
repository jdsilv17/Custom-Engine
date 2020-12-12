

Texture2D textures[2] : register(t0);
SamplerState samplerState : register(s0);

cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4 LightPos[3];
    float4 LightDir[3];
    float4 LightColor[3];
    float4 OutputColor;
}

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse);
float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse);
float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse);
float CalcLinearAttenuation(float4 lPos, float lRadius, float4 sPos);
float CalcLinearAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio);

float4 PS_MultiTexturing(PS_INPUT input);
float4 PS_SingleTexture(PS_INPUT input);
float4 PS_Solid(PS_INPUT input);

float4 main(PS_INPUT input) : SV_TARGET
{
    
    float4 texColor = PS_MultiTexturing(input);
    float4 finalColor = 0;
    finalColor = CalcDirectinalLight(LightDir[0].xyz, LightColor[0], input.Normal, texColor);
    
    float pntRadius = 35.0f;
    finalColor += CalcPointLight(LightPos[1], LightColor[1], pntRadius, input.wPos, input.Normal, texColor);
    
    finalColor += CalcSpotLight(LightPos[2], (float3) LightDir[2], LightColor[2], input.wPos, input.Normal, texColor);
    
    return finalColor;
}

float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse)
{
    if (length(lDir) != 1.0f)
        lDir = normalize(lDir);
    
    float ambientTerm = 0.5f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = clamp((dot(-lDir, sNormal) + ambientTerm), 0.0f, 5.0f);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation;
    
    return finalColor;
}

float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse)
{
    float4 lightDir = lPos - sPos;
    float dist = length(lightDir);
    lightDir = normalize(lightDir);
    lightDir /= dist;
    
    float ambientTerm = 0.9f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = clamp((dot(lightDir.xyz, sNormal) + ambientTerm), 0.0f, 5.0f);
    
    float rangeAttenuation = pow(1.0f - (dist / lRadius), 2.0f); // lightrange
    
    float linearAttenuation = CalcLinearAttenuation(lPos, lRadius, sPos);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * rangeAttenuation * linearAttenuation;
    
    return finalColor;
}

float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse)
{
    float4 lightDir = normalize(lPos - sPos);
    
    float ambientTerm = 0.1f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = clamp((dot(lightDir.xyz, sNormal) + ambientTerm), 0.0f, 5.0f);
    
    float surfaceRatio = dot(-lightDir.xyz, coneDir);
    
    float outerConeRatio = 0.5f;
    float innerConeRatio = 0.8f;
    float linearAttenuation = CalcLinearAttenuation(innerConeRatio, outerConeRatio, surfaceRatio);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * linearAttenuation;
    
    return finalColor;
}

float CalcLinearAttenuation(float4 lPos, float lRadius, float4 sPos)
{
    //float attenuation = length(lPos - sPos);
    //attenuation = attenuation / lRadius;
    //attenuation = saturate(attenuation);
    //attenuation = 1.0f - attenuation;
    //attenuation = pow(attenuation, 2.0f);
    float attenuation = pow(1.0f - saturate(length(lPos - sPos) / lRadius), 2.0f);
    return attenuation;
}

float CalcLinearAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio)
{
    float attenuation = pow(1.0f - saturate((innerConeRatio - surfaceRatio) / (innerConeRatio - outerConeRatio)), 2.0f);
    return attenuation;
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