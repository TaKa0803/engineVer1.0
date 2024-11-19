#include "CopyImage.hlsli"
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float32_t4 ApplyGaussianBlur(float32_t2 uv)
{
    // 重み係数
    float32_t weight[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
    float32_t4 blurredColor = float32_t4(0, 0, 0, 0);

    //画像サイズ
    float32_t2 texelSize;
    
    texelSize.x = 1.0f / 1280.0f;
    texelSize.y = 1.0f / 720.0f;
    
    // 水平方向のブラー
    for (int i = -2; i <= 2; i++)
    {
        blurredColor += gTexture.Sample(gSampler, uv + float32_t2(texelSize.x * i, 0)) * weight[abs(i)];
    }

    // 垂直方向のブラー
    float32_t4 finalBlur = float32_t4(0, 0, 0, 0);
    for (int ii = -2; ii <= 2; ii++)
    {
        finalBlur += blurredColor * weight[abs(ii)] * gTexture.Sample(gSampler, uv + float32_t2(0, texelSize.y * ii));
    }

    return finalBlur;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // Step 2: 高輝度部分に対してガウシアンブラーを適用
    float32_t4 blurredHighLuminance = ApplyGaussianBlur(input.texcoord);

    output.color = blurredHighLuminance;
    return output;
}