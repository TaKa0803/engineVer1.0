#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float32_t2 center;
    int32_t numSample;
    float32_t BlurWidth;
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
    
    //ラディアルブラー
    const float32_t2 kCenter = gMaterial.center;
    const int32_t kNumSamples = gMaterial.numSample; //多いほど滑らかだが重い
    const float32_t kBlurWidth = gMaterial.BlurWidth;
    
    float32_t2 direction = input.texcoord - kCenter;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex)
    {
        //現在のUVから先ほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float32_t2 texcoord = input.texcoord + direction * kBlurWidth * float32_t(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    //平均化する
    outputColor.rgb *= rcp(kNumSamples);
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    
    return output;
}