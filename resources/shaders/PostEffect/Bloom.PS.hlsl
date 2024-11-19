#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

Texture2D<float32_t4> gTex : register(t1);


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

    //各色取得
    float32_t4 lumi = gTexture.Sample(gSampler, input.texcoord);
    float32_t4 base = gTex.Sample(gSampler, input.texcoord);
   
    output.color = base + (lumi * gMaterial.value);
    
    return output;
}