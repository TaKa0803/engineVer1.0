#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float32_t> gMaskTexture : register(t1);

struct Material
{
    float32_t value;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    if (mask <= gMaterial.value)
    {
        discard;
    }
    
    float32_t edge = 1.0f - smoothstep(gMaterial.value, gMaterial.value+0.03f, mask);    
    
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //Edgeぽいほど指定した色を加算
    output.color.rgb += edge * float32_t3(1.0f, 0.4f, 0.3f);
    
    return output;
}