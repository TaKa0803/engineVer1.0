#include "CopyImage.hlsli"

//加工された画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);
//未加工の画像
Texture2D<float32_t4> gTex : register(t1);

//マテリアル
struct Material
{
    //エフェクトの効果量0-1
    float32_t value;
};
//マテリアルのコンスタントバッファ作成
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセルシェーダの出力物
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

//実際の処理
PixelShaderOutput main(VertexShaderOutput input)
{
    //返却変数作成
    PixelShaderOutput output;

    //各色取得
    //高輝度をとった画像取得
    float32_t4 lumi = gTexture.Sample(gSampler, input.texcoord);
    //元画像を取得
    float32_t4 base = gTex.Sample(gSampler, input.texcoord);
   
    //元画像と、効果量をかけた高輝度画像を足す
    output.color = base + (lumi * gMaterial.value);
    
    //結果を返却
    return output;
}