#include "CopyImage.hlsli"

//シーンの画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//マテリアル
struct Material
{
    float32_t value;//効果量
};
//コンスタントバッファで作成
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセルの出力
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

//円周率設定
static const float32_t PI = 3.14159265f;

//ガウス処理
float gauss(float x, float y, float sigma)
{
    float exponent = -(x * x + y * y) * rcp(2.0f * sigma * sigma);
    float denominator = 2.0f * PI * sigma * sigma;
    return exp(exponent) * rcp(denominator);
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //画像の色取得
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //kenrnelを求める、weightはあと
    float32_t weight = 0.0f;
    
    //奇数で作成
    const int kernel = 9;
    
    //インデックス作成
    float32_t2 kIndex[kernel][kernel];
    //インデックス計算
    for (int32_t ky = 0; ky < kernel; ++ky) {
        for (int32_t kx = 0; kx < kernel; ++kx) {

            float32_t xNum = (-(kernel - 1) / 2) + kx;
            float32_t yNum = (-(kernel - 1) / 2) + ky;
            
            kIndex[ky][kx] = float32_t2( xNum, yNum );

        }
    }
    
    //カーネル作成
     float32_t kernel5x[kernel][kernel];
    for (int32_t x = 0; x < kernel; ++x){
        for (int32_t y = 0; y < kernel; ++y){
            kernel5x[x][y] = gauss(kIndex[x][y].x, kIndex[x][y].y, gMaterial.value); //最終引数が標準偏差
            weight += kernel5x[x][y];
        }
    }

    uint32_t width, height; //uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        
    output.color.rgb = float32_t3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
        
    //ループ
    for (int32_t xx = 0; xx < kernel; ++xx)
    {
        for (int32_t yy = 0; yy < kernel; ++yy)
        {
                //現在のtexcoord算出
            float32_t2 texcoord = input.texcoord + kIndex[xx][yy] * uvStepSize;
                //色に1/9かけてタス
            float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kernel5x[xx][yy];
        }
    }
        
    
    output.color.rgb *= rcp(weight);
    
    return output;
}