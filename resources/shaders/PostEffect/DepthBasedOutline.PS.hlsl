#include "CopyImage.hlsli"

//画面の画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//深度地の入った画像
Texture2D<float32_t> gDepthTexture : register(t1);
//サンプラー
SamplerState gSamplerPoint : register(s1);

//マテリアル
struct Material
{
    float32_t4x4 projectionInverse; //プロジェクション逆行列
    float32_t value;                //値
    int32_t enableColor;            //画像の有効化フラグ
    
};
//コンスタントバッファ設定
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセル出力
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

static const float32_t kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

float32_t Luminance(float32_t3 v)
{
    return dot(v, float32_t3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutput main(VertexShaderOutput input)
{
    //Smoothingの処理
    uint32_t width, height; //uvStepSizeの算出
    //ディメンションの取得
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
          
    //縦横それぞれの畳み込みの結果を収納する
    float32_t2 difference = float32_t2(0.0f, 0.0f);
    
    //色を光度に変換して畳み込みを行っていく。微分filter要のKernelになっているので今までと同じ
    for (int32_t x = 0; x < 3; ++x)
    {
        for (int32_t y = 0; y < 3; ++y)
        {
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float32_t ndcDepth = gDepthTexture.Sample(gSamplerPoint, texcoord);
            //NDC -> View
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gMaterial.projectionInverse);
            float32_t viewZ = viewSpace.z * rcp(viewSpace.w);//同次座標系からデカルト座標系へ変換
            difference.x += viewZ * kPrewittHorizontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }      
    }
    //重さを計算
    float weight = length(difference);
   
    PixelShaderOutput output;
    
    //画像が無効の場合
    if (!gMaterial.enableColor)
    {
        //アウトラインのみの描画
        weight = saturate(weight * gMaterial.value);
        output.color.rgb = weight;
    }
    else
    {
        //画像にアウトラインを含め描画
        weight = saturate(weight * gMaterial.value);
        output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    }
    //透明度は固定
    output.color.a = 1.0f;
    
    return output;
}