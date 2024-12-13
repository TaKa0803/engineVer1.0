#include "CopyImage.hlsli"

//画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//ピクセルシェーダの出力物
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //画像をそのまま出力
    output.color = gTexture.Sample(gSampler, input.texcoord);
    return output;
}