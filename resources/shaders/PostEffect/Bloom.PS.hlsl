#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);



struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float32_t4 CheckHighLuminance(float32_t4 color)
{
    float32_t3 pColor = float32_t3(0.2125f, 0.7154f, 0.0721f);
    float32_t grayScale = dot(color.rgb, pColor);
    
    float32_t extract = smoothstep(0.6f, 0.9f, grayScale);
    
    return color * extract;
}


float Gaussian(float32_t2 drawUV, float32_t2 pickUV, float32_t sigma)
{
    float32_t d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float32_t4 Blur(float32_t2 uv)
{
    float32_t totalWeight = 0, sigma = 0.005f, stepWidth = 0.001f;
    float32_t4 col = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (float32_t py = -sigma * 2.0f; py <= sigma * 2; py += stepWidth)
    {
        for (float32_t px = -sigma * 2.0f; px <= sigma * 2; px += stepWidth)
        {
            float32_t2 pickUV = uv + float32_t2(px, py);
            float32_t weight = Gaussian(uv, pickUV, sigma);
            col += gTexture.Sample(gSampler, pickUV) * weight;
            totalWeight += weight;
        }
    }
    
    col.rgb = col.rgb / totalWeight;
    return col;
}


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    //元画像の色
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
 
    //ブラーをかけた高高度のみの画像
    float32_t4 highLumi = Blur(input.texcoord);
   
    output.color = (color + highLumi);
    
    return output;
}