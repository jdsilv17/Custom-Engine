

Texture2D textures[2] : register(t0);
SamplerState samplerState : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    //float4 Color : COLOR;
    float3 UVW : TEXCOORD;
    float3 Normal : NORMAL;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    
    // get first color for pixel
    float4 baseColor = textures[0].Sample(samplerState, input.UVW.xy);
    // get second color for pixel
    float4 detailColor = textures[1].Sample(samplerState, input.UVW.xy);
    // blend the pixel's color together
    //float4 blendColor = baseColor * detailColor * 2.0f;
    
    float4 finalColor /*= saturate(blendColor)*/;
    finalColor = float4(lerp(baseColor.rgb, detailColor.rgb, detailColor.a), baseColor.a);
    //finalColor = float4(lerp(color[0].rgb, color[1].rgb, color[1].a), color[0].a);
    
    
    return finalColor;
}

float4 PS_SingleTexture(PS_INPUT input) : SV_Target
{
    float4 baseColor = textures[0].Sample(samplerState, input.UVW.xy);
    return baseColor;
}

float4 PS_Solid(PS_INPUT input) : SV_Target
{
    float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    return color;
}