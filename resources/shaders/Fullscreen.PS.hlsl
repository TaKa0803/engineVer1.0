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
        output.color.rgb = gMaterial.sepiaValue + float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
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
   
    return output;
}