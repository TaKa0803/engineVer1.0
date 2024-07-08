#include "CopyImage.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Material
{
    float32_t4 color1;
    float32_t4 color2;
    float32_t value;
    float32_t time;
    float32_t discardNum;
};
ConstantBuffer<Material> gMaterial : register(b0);


float32_t rand2dTo1d(float32_t2 value, float32_t2 dotDir = float32_t2(12.9898, 78.233))
{
    float2 smallValue = sin(value);
    float random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
   
    
    float32_t random = rand2dTo1d(input.texcoord*gMaterial.time);
   
    random = clamp(random, gMaterial.value, 1.0f);
    float32_t r = lerp(gMaterial.color1.r, gMaterial.color2.r, random);
    float32_t g = lerp(gMaterial.color1.g, gMaterial.color2.g, random);
    float32_t b = lerp(gMaterial.color1.b, gMaterial.color2.b, random);
   
    if (r < gMaterial.discardNum)
    {
        r = 1;
    }
    if (g < gMaterial.discardNum)
    {
        g = 1;
    }
    if (b < gMaterial.discardNum)
    {
        b = 1;
    }
    
    //色にする
    output.color *= float32_t4(r,g,b,1);
   
    
    
    return output;
}