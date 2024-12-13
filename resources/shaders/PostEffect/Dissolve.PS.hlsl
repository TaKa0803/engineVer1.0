#include "CopyImage.hlsli"

//元画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//マスク画像
Texture2D<float32_t> gMaskTexture : register(t1);

//マテリアル
struct Material
{
    float32_t4 color;       //色
    float32_t4 edgeColor;   //境界線の色
    float32_t edgeValue;    //境界線の太さ
    float32_t value;        //マスク量
};
//コンスタントバッファに作成
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセル出力物
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    //マスク画像読み込み
    float32_t mask = gMaskTexture.Sample(gSampler, input.texcoord);
    
    //マスク量より小さい場合
    if (mask <= gMaterial.value)
    {
        //元画像の値になる
        output.color=gMaterial.color;
       
        //終了
        return output;
    }
    
    //境界線
    float32_t edge = 1.0f - smoothstep(gMaterial.value, gMaterial.value+gMaterial.edgeValue, mask);    
    
    //元画像読み込み
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    //Edgeぽいほど指定した色を加算
    output.color.rgb += edge * float32_t3(gMaterial.edgeColor.r, gMaterial.edgeColor.g, gMaterial.edgeColor.b);
    
    return output;
}