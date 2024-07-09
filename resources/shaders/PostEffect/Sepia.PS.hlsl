#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float32_t value;
    float32_t effectiveValue;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //セピア（value変更で明るさ
    output.color.rgb *= gMaterial.value * lerp(float32_t3(1.0f, 1.0f, 1.0f), float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f),gMaterial.effectiveValue);
    return output;
}