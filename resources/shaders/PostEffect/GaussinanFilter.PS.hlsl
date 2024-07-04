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

static const float32_t2 kIndex10x10[9][9] =
{
    { { -4.0f, -4.0f }, { -3.0f, -4.0f }, { -2.0f, -4.0f }, { -1.0f, -4.0f }, { 0.0f, -4.0f }, { 1.0f, -4.0f }, { 2.0f, -4.0f }, { 3.0f, -4.0f }, { 4.0f, -2.0f } },
    { { -4.0f, -3.0f }, { -3.0f, -3.0f }, { -2.0f, -3.0f }, { -1.0f, -3.0f }, { 0.0f, -3.0f }, { 1.0f, -3.0f }, { 2.0f, -3.0f }, { 3.0f, -3.0f }, { 4.0f, -2.0f } },
    { { -4.0f, -2.0f }, { -3.0f, -2.0f }, { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f }, { 3.0f, -2.0f }, { 4.0f, -2.0f } },
    { { -4.0f, -1.0f }, { -3.0f, -1.0f }, { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f }, { 3.0f, -1.0f }, { 4.0f, -1.0f } },
    { { -4.0f, 0.0f },  { -3.0f, 0.0f },  { -2.0f, 0.0f },  { -1.0f, 0.0f },  { 0.0f, 0.0f },  { 1.0f, 0.0f },  { 2.0f, 0.0f },  { 3.0f, 0.0f },  { 4.0f, 0.0f } },
    { { -4.0f, 1.0f },  { -3.0f, 1.0f },  { -2.0f, 1.0f },  { -1.0f, 1.0f },  { 0.0f, 1.0f },  { 1.0f, 1.0f },  { 2.0f, 1.0f },  { 3.0f, 1.0f },  { 4.0f, 1.0f } },
    { { -4.0f, 2.0f },  { -3.0f, 2.0f },  { -2.0f, 2.0f },  { -1.0f, 2.0f },  { 0.0f, 2.0f },  { 1.0f, 2.0f },  { 2.0f, 2.0f },  { 3.0f, 2.0f },  { 4.0f, 2.0f } },
    { { -4.0f, 3.0f },  { -3.0f, 3.0f },  { -2.0f, 3.0f },  { -1.0f, 3.0f },  { 0.0f, 3.0f },  { 1.0f, 3.0f },  { 2.0f, 3.0f },  { 3.0f, 3.0f },  { 4.0f, 3.0f } },
    { { -4.0f, 4.0f },  { -3.0f, 4.0f },  { -2.0f, 4.0f },  { -1.0f, 4.0f },  { 0.0f, 4.0f },  { 1.0f, 4.0f },  { 2.0f, 4.0f },  { 3.0f, 4.0f },  { 4.0f, 4.0f } },
  
};


static const float32_t PI = 3.14159265f;

float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //kenrnelを求める、weightはあと
    float32_t weight = 0.0f;
    float32_t kernel5x[10][10];
    for (int32_t x = 0; x < 10; ++x)
    {
        for (int32_t y = 0; y < 10; ++y)
        {
            kernel5x[x][y] = gauss(kIndex10x10[x][y].x, kIndex10x10[x][y].y, gMaterial.value); //最終引数が標準偏差
            weight += kernel5x[x][y];
        }
    }

    uint32_t width, height; //uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
        
        //5x5ループ
    for (int32_t xx = 0; xx < 10; ++xx)
    {
        for (int32_t yy = 0; yy < 10; ++yy)
        {
                //現在のtexcoord算出
            float32_t2 texcoord = input.texcoord + kIndex10x10[xx][yy] * uvStepSize;
                //色に1/9かけてタス
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel5x[xx][yy];
        }
    }
        
    output.color.rgb *= rcp(weight);
    
    return output;
}