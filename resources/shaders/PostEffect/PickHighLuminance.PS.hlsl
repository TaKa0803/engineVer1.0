#include "CopyImage.hlsli"
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


float32_t4 CheckHighLuminance(float32_t4 color,float32_t min,float32_t max)
{
    float32_t3 pColor = float32_t3(0.2125f, 0.7154f, 0.0721f);
    float32_t grayScale = dot(color.rgb, pColor);
    
    float32_t extract = smoothstep(min, max, grayScale);
    
    return color * extract;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t min=0.1f;
    float32_t max=0.9f;
    // Step 1: 元の画像から高輝度部分を抽出
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    float32_t4 highLuminanceColor = CheckHighLuminance(color,min,max);

    output.color = highLuminanceColor;
    return output;
}
