#include "ConstantBuffers.hlsli"

Texture2D textures[3] : register(t0);
SamplerState samplerState : register(s0);

float4 main(PS_DEFAULT_INPUT input) : SV_TARGET
{
    float4 amientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    float4 finalColor = 0;
    float4 tex[3]; // diffuse, emissive, specular
    for (int i = 0; i < 3; ++i)
    {
        tex[i] = PS_SingleTexture(input, textures[i], samplerState);
    }
    finalColor = tex[0] * amientColor + tex[1];
    finalColor += CalcDirectinalLight(LightDir[0].xyz, LightColor[0], input.Normal, tex[0]);
    
    float pntRadius = 5.0f;
    finalColor += CalcPointLight(LightPos[1], LightColor[1], pntRadius, input.wPos, input.Normal, tex[0], tex[2]);
    
    //finalColor += CalcSpotLight(LightPos[2], (float3) LightDir[1], LightColor[2], input.wPos, input.Normal, texColor);
    
    return saturate(finalColor);
}

//PS_OUTPUT main(VSOut input)
//{
//    PS_OUTPUT output;
//    float3 light_dir = light_pos - input.world_pos.xyz;
//    float sq_distance = dot(light_dir, light_dir);
//   
//    light_dir = light_dir / sqrt(sq_distance);
//    float3 eye_dir = input.eye_pos.xyz - input.world_pos.xyz;
//    float sq_distance_eye = dot(eye_dir, eye_dir);
//    float distance_eye = sqrt(sq_distance_eye);
//    eye_dir = eye_dir / distance_eye;
//    float3 norm = normalize(input.normal.xyz);
//    float nl = dot(norm, light_dir);
//    float diffuse_intensity = saturate(nl);
//    float3 half_vector = normalize(light_dir + eye_dir);
//    float nh = dot(norm, half_vector);
//    float specular_intensity = pow(saturate(nh), 1 + surface_shininess);
//    float4 light_intensity = float4(light_color, 0.0f) * light_power / sq_distance;
//    float4 mat_diffuse = tx_diffuse.Sample(samLinear, input.uv); // *float4(surface_diffuse, 0.0f) * surface_diffuse_factor;
//    float4 mat_specular = tx_specular.Sample(samLinear, input.uv); // *float4(surface_specular, 0.0f) * surface_specular_factor;
//    float4 mat_emissive = tx_emissive.Sample(samLinear, input.uv); // *float4(surface_emissive, 0.0f) * surface_emissive_factor;
//    float4 emissive = mat_emissive;
//    float4 ambient = mat_diffuse * ambient_light;
//    float4 specular = mat_specular * specular_intensity * light_intensity;
//    float4 diffuse = mat_diffuse * diffuse_intensity * light_intensity;
//    // hacky conservation of energy
//    diffuse.xyz -= specular.xyz;
//    diffuse.xyz = saturate(diffuse.xyz);
//    float4 color = ambient + specular + diffuse + emissive;
//     
//    output.color = color;
//    return output;
//}