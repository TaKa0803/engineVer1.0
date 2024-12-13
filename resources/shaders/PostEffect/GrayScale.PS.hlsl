#include "CopyImage.hlsli"

//シーンの画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//マテリアル
struct Material
{
    float32_t value;//影響量
};
//コンスタントバッファで作成
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセルの出力
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //画像の色取得
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //グレースケール処理
    float32_t3 pColor = float32_t3(0.2125f, 0.7154f, 0.0721f);
    //グレースケールをかけたものを作成
    float32_t value = dot(output.color.rgb, pColor);
    //強度を設定
    output.color.rgb = lerp(float32_t3(output.color.rgb), float32_t3(value, value, value),gMaterial.value);
   
    return output;
 }