
#include "ConstantBuffers.hlsli"

//#pragma pack_matrix(row_major)

// rule of 3 

PS_ANIM_INPUT main(VS_ANIM_INPUT input)
{
    PS_ANIM_INPUT output = (PS_ANIM_INPUT) 0;
    output.Pos = 0;
    output.Color = input.Color;
    output.UV = input.UV;

    
    output.Pos = mul(input.Pos, SkinMats[input.Joints.x]) * input.Weights.x;
    output.Pos += mul(input.Pos, SkinMats[input.Joints.y]) * input.Weights.y;
    output.Pos += mul(input.Pos, SkinMats[input.Joints.z]) * input.Weights.z;
    output.Pos += mul(input.Pos, SkinMats[input.Joints.w]) * input.Weights.w;
    
    //output.Normal  = (mul(input.Normal, (float3x3)SkinMats[input.Joints.x]) * input.Weights.x).xyz;
    //output.Normal += (mul(input.Normal, (float3x3)SkinMats[input.Joints.y]) * input.Weights.y).xyz;
    //output.Normal += (mul(input.Normal, (float3x3)SkinMats[input.Joints.z]) * input.Weights.z).xyz;
    //output.Normal += (mul(input.Normal, (float3x3)SkinMats[input.Joints.w]) * input.Weights.w).xyz;
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), World)).xyz;
    
    
    output.Pos = mul(output.Pos, World);
    output.wPos = output.Pos;
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    
    return output;
}