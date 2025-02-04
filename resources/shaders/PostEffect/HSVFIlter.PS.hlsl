#include "CopyImage.hlsli"

//シーンの画像
Texture2D<float32_t4> gTexture : register(t0);
//サンプラー
SamplerState gSampler : register(s0);

//マテリアル
struct Material
{
    float32_t hue;          //H
    float32_t saturation;   //S
    float32_t value;        //V
    
    float32_t effective;    //効果量
};
//コンスタントバッファで作成
ConstantBuffer<Material> gMaterial : register(b0);

//ピクセルの出力
struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

//RGBをHSVにする
float32_t3 RGBToHSV(float32_t3 rgb)
{
    //RGBを取り出す
    float r, g, b;
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
    
    
    float max = r > g ? r : g;
    max = max > b ? max : b;
    float min = r < g ? r : g;
    min = min < b ? min : b;
    float h = max - min;
    
    if (h > 0.0f)
    {
        if (max == r)
        {
            h = (g - b) / h;
            if (h < 0.0f)
            {
                h += 6.0f;
            }
        }
        else if (max == g)
        {
            h = 2.0f + (b - r) / h;
        }
        else
        {
            h = 4.0f + (r - g) / h;
        }
    }
    h /= 6.0f;
    float s = (max - min);
    if (max != 0.0f)
        s /= max;
    float v = max;
    
    return float32_t3(h, s, v);
    
};

//HSVをRGBにする
float32_t3 HSVToRGB(float32_t3 hsv)
{
    float h,s,v;
    h = hsv.r;
    s = hsv.g;
    v = hsv.b;
    
    float r = v;
    float g = v;
    float b = v;
    if (s > 0.0f)
    {
        h *= 6.0f;

        int i = (int) h;
 
        float f = h - (float) i;
        switch (i)
        {
            default:
            case 0:
                g *= 1 - s * (1 - f);
                b *= 1 - s;
                break;
            case 1:
                r *= 1 - s * f;
                b *= 1 - s;
                break;
            case 2:
                r *= 1 - s;
                b *= 1 - s * (1 - f);
                break;
            case 3:
                r *= 1 - s;
                g *= 1 - s * f;
                break;
            case 4:
                r *= 1 - s * (1 - f);
                g *= 1 - s;
                break;
            case 5:
                g *= 1 - s;
                b *= 1 - s * f;
                break;
        }
    }
    
    return float32_t3(r, g, b);
};

float32_t WarpValue(float32_t value, float32_t minRange, float32_t maxRange)
{
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, range);
    if (modValue < 0)
    {
        modValue += range;
    }
    return minRange + modValue;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
   float32_t4 texColor = gTexture.Sample(gSampler, input.texcoord);
    
    float32_t3 hsv = RGBToHSV(texColor.rgb);
    
    //ここで調整する
    hsv.x += gMaterial.hue;
    hsv.y += gMaterial.saturation;
    hsv.z += gMaterial.value;
    
    hsv.x = WarpValue(hsv.x, 0.0f, 1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);
    
    
    float32_t3 rgb = HSVToRGB(hsv);
    
    output.color.rgb = lerp(texColor.rgb, rgb, gMaterial.effective);
    output.color.a = texColor.a;
    
    return output;
}