cbuffer ConstantBuffer : register(b0) // b for buffer
{
    float4x4 World;
    float4x4 View;
    float4x4 Projection;
    float4x4 SkinMats[28];
    //float4x4 SkinNormalMats[6];
    float4 LightPos[3];
    float4 LightDir[3];
    float4 LightColor[3];
    float4 camPos;
    float4 totalTime;
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

struct VS_ANIM_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
    int4 Joints : JOINT_INDICES;
    float4 Weights : WEIGHTS;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct VS_COLOR_INPUT
{
    float4 Pos : POSITION;
    float4 Color : COLOR;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

struct PS_DEFAULT_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
};

struct PS_ANIM_INPUT
{
    float4 Pos : SV_POSITION;
    float4 wPos : POSITION;
    float4 Color : COLOR;
    int4 Joints : JOINT_INDICES;
    float4 Weights : WEIGHTS;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
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

struct GS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 UVW : TEXCOORD;
};


// PS Functions============================================================
float4 CalcDirectinalLight(float3 lDir, float4 lColor, float3 sNormal, float4 tDiffuse);
float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse);
float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse, float4 specDiffuse);
float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse);
float CalcLinearAttenuation(float4 lPos, float lRadius, float4 sPos);
float CalcLinearAttenuation(float innerConeRatio, float outerConeRatio, float surfaceRatio);
float4 CalcSpecularComponent(float4 lColor, float3 lDir, float3 sPos, float3 sNormal, float3 camPos, float specularPower, float specularIntensity);


float4 PS_MultiTexturing(PS_INPUT input, Texture2D tex[2], SamplerState samplerState);
float4 PS_SingleTexture(float2 uv, Texture2D tex, SamplerState samplerState);
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
    float3 lightDir = lPos.xyz - sPos.xyz;
    float dist = length(lightDir);
    lightDir = normalize(lightDir);
    lightDir /= dist;
    
    // Specular Component
    float4 ReflectedLight = 0;
    ReflectedLight = CalcSpecularComponent(lColor, lightDir, sPos.xyz, sNormal, camPos.xyz, 10.0f, 0.8f);
    
    float ambientTerm = 1.0f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = saturate((dot(lightDir, sNormal) + ambientTerm));
    
    float rangeAttenuation = pow(1.0f - (dist / lRadius), 2.0f); // lightrange
    
    float linearAttenuation = CalcLinearAttenuation(lPos, lRadius, sPos);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * rangeAttenuation * linearAttenuation + ReflectedLight;
    
    return finalColor;
}

float4 CalcPointLight(float4 lPos, float4 lColor, float lRadius, float4 sPos, float3 sNormal, float4 tDiffuse, float4 specDiffuse)
{
    float3 lightDir = lPos.xyz - sPos.xyz;
    float dist = length(lightDir);
    lightDir = normalize(lightDir);
    lightDir /= dist;
    
    // Specular Component
    float4 ReflectedLight = 0;
    ReflectedLight = CalcSpecularComponent(lColor, lightDir, sPos.xyz, sNormal, camPos.xyz, 10.0f, 0.5f);
    ReflectedLight *= specDiffuse;
    
    float ambientTerm = 0.0f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = saturate((dot(lightDir, sNormal) + ambientTerm));
    
    float rangeAttenuation = pow(1.0f - (dist / lRadius), 2.0f); // lightrange
    
    float linearAttenuation = CalcLinearAttenuation(lPos, lRadius, sPos);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * rangeAttenuation * linearAttenuation + ReflectedLight;
    
    return finalColor;
}

float4 CalcSpotLight(float4 lPos, float3 coneDir, float4 lColor, float4 sPos, float3 sNormal, float4 tDiffuse)
{
    float3 lightDir = normalize(lPos.xyz - sPos.xyz);

    // Specular Component
    float4 ReflectedLight = 0;
    ReflectedLight = CalcSpecularComponent(lColor, lightDir, sPos.xyz, sNormal, camPos.xyz, 5.0f, 1.0f);
    
    float ambientTerm = 0.1f;
    //float4 ambientColor = lColor * ambientTerm;
    
    float angularAttenuation = saturate((dot(lightDir, sNormal) + ambientTerm));
    
    float surfaceRatio = dot(-lightDir, coneDir);
    
    float outerConeRatio = 0.5f;
    float innerConeRatio = 0.8f;
    float linearAttenuation = CalcLinearAttenuation(innerConeRatio, outerConeRatio, surfaceRatio);
    
    float4 finalColor = tDiffuse * lColor * angularAttenuation * linearAttenuation + ReflectedLight;
    
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

float4 CalcSpecularComponent(float4 lColor, float3 lDir, float3 sPos, float3 sNormal, float3 camPos, float specularPower, float specularIntensity)
{
    float3 viewDir = normalize(camPos - sPos);
    float3 halfVector = normalize(-lDir + viewDir);
    float intensity = max(pow(saturate(dot(sNormal, halfVector)), specularPower), 0.0f);
    
    return lColor * intensity * specularIntensity;
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

float4 PS_SingleTexture(float2 uv, Texture2D tex, SamplerState samplerState)
{
    float4 baseColor = tex.Sample(samplerState, uv);
    return baseColor;
}

float4 PS_Solid(PS_INPUT input)
{
    float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    return color;
}