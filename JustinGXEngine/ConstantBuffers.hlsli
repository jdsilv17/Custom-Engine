cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4 LightPos[3];
    float4 LightDir[3];
    float4 LightColor[3];
    float4 camPos;
    float4 OutputColor;
}

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct VS_SKY_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 UVW : TEXCOORD;
};

struct VS_DEFAULT_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_SOLID_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

struct PS_SKY_INPUT
{
    float4 Pos : SV_POSITION;
    float3 LPos : POSTION;
    float3 Normal : NORMAL;
    float3 UVW : TEXCOORD;
};


// PS Functions============================================================
float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse);
float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse);
float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse);
float CalcLinearAttenuation(float4 lPos, float lRadius, float4 sPos);
float CalcLinearAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio);


float4 PS_MultiTexturing(PS_INPUT input, Texture2D tex[2], SamplerState samplerState);
float4 PS_SingleTexture(PS_INPUT input, Texture2D tex, SamplerState samplerState);
float4 PS_Solid(PS_INPUT input);

float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse)
{
    if (length(lDir) != 1.0f)
        lDir = normalize(lDir);
    
    float ambientTerm = 0.5f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = saturate((dot(-lDir, sNormal) + ambientTerm));
    
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
    
    float angularAttenuation = saturate((dot(lightDir.xyz, sNormal) + ambientTerm));
    
    float rangeAttenuation = pow(1.0f - (dist / lRadius), 2.0f); // lightrange
    
    float linearAttenuation = CalcLinearAttenuation(lPos, lRadius, sPos);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * rangeAttenuation * linearAttenuation;
    
    return finalColor;
}

float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse)
{
    float4 lightDir = normalize(lPos - sPos);
    // Specular Component
    //float4 viewDir = normalize(camPos - sPos);
    //float4 halfVector = normalize(-lightDir + viewDir);
    //float specularPower = 2.0f;
    //float intensity = max(pow( /*clamp(*/dot(sNormal, (float3) halfVector) /*)*/, specularPower), 0);

    float ambientTerm = 0.1f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = saturate((dot(lightDir.xyz, sNormal) + ambientTerm));
    
    float surfaceRatio = dot(-lightDir.xyz, coneDir);
    
    float outerConeRatio = 0.5f;
    float innerConeRatio = 0.8f;
    float linearAttenuation = CalcLinearAttenuation(innerConeRatio, outerConeRatio, surfaceRatio);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * linearAttenuation;
    
    return finalColor;
}

float CalcLinearAttenuation(float4 lPos, float lRadius, float4 sPos)
{
    float attenuation = pow(1.0f - saturate(length(lPos - sPos) / lRadius), 2.0f);
    return attenuation;
}

float CalcLinearAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio)
{
    float attenuation = pow(1.0f - saturate((innerConeRatio - surfaceRatio) / (innerConeRatio - outerConeRatio)), 2.0f);
    return attenuation;
}

float4 PS_MultiTexturing(PS_INPUT input, Texture2D tex[2], SamplerState samplerState)
{
    
    // get first color for pixel
    float4 baseColor = tex[0].Sample(samplerState, input.UVW.xy);
    // get second color for pixel
    float4 detailColor = tex[1].Sample(samplerState, input.UVW.xy);
    // blend the pixel's color together
    //float4 blendColor = baseColor * detailColor * 2.0f;
    
    //float4 finalColor = saturate(blendColor);
    float4 finalColor = float4(lerp(baseColor.rgb, detailColor.rgb, detailColor.a), baseColor.a);
    
    clip(finalColor.a);
    
    return finalColor;
}

float4 PS_SingleTexture(PS_INPUT input, Texture2D tex, SamplerState samplerState)
{
    float4 baseColor = tex.Sample(samplerState, input.UVW.xy);
    return baseColor;
}

float4 PS_Solid(PS_INPUT input)
{
    float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    return color;
}