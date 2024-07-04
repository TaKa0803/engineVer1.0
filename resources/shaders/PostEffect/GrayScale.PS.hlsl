#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

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
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //グレースケール
    float32_t3 pColor = gMaterial.value * float32_t3(0.2125f, 0.7154f, 0.0721f);
    float32_t value = dot(output.color.rgb, pColor);
    output.color.rgb = float32_t3(value, value, value);
   
    return output;
 }