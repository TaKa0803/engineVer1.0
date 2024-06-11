#include "Fullscreen.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    int32_t type;
    float32_t sepiaValue;
};
ConstantBuffer<Material> gMaterial : register(b0);


struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

static const float32_t kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }  
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
   
    //タイプによる処理
    if (gMaterial.type == 1)
    {
    //グレースケール
        float32_t3 pColor = { 0.2125f, 0.7154f, 0.0721f };
        float32_t value = dot(output.color.rgb, pColor);
        output.color.rgb = float32_t3(value, value, value);
    }
    else if (gMaterial.type == 2)
    {
        //セピア（value変更で明るさ
        output.color.rgb *= gMaterial.sepiaValue * float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
    }
    else if (gMaterial.type == 3)
    {
        //Vignetting
        // 周囲をΘに、中心になるほど明るくなる
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
         //correctだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleを調整
        float vignette = correct.x * correct.y * 16.0f;
        //とりあえず0.8倍してみた
        vignette = saturate(pow(vignette, 0.2f));
    
        output.color.rgb *= vignette;
    }
    else if (gMaterial.type == 4)
    {
        //Smoothing
        uint32_t width, height;//uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
        output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
        output.color.a = 1.0f;
        
        //3x3ループ
        for (int32_t x = 0; x < 3; ++x)
        {
            for (int32_t y = 0; y < 3; ++y)
            {
                //現在のtexcoord算出
                float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                //色に1/9かけてタス
                float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                output.color.rgb += fetchColor * kKernel3x3[x][y];
            }
        }

    }
   
    return output;
}