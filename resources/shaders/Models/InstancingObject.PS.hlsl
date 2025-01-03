#include"InstancingObject.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


struct Material {
    float32_t4 color;
    
    float32_t4x4 uvTransform;
    
    int32_t enableLighting;
    
    int32_t enableTexture;
    
    int32_t enableHalfLambert;
    
    float32_t discardNum;
    
    int32_t enablePhongReflection;
    
    float32_t shininess;
    
    int32_t enablePointLight;
    
    int32_t enableEnvironmentMap;
    
    float enviromentCoefficient;
};
ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

struct DirectionalLight {
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

//カメラの行列
struct Camera
{
    float32_t3 worldPosition;
};

ConstantBuffer<Camera> gCamera : register(b2);


//ポイントライト
struct PointLight
{
    float32_t4 color;
    float32_t3 position;
    float intensity;
    float radius;
    float decay;
};

ConstantBuffer<PointLight> gPointLight : register(b3);



PixelShaderOutput main(VertexShaderOutput input) {
    
    float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
   
    
    //画像有効フラグが無効の時マテリアルの色のみ有効
    if (gMaterial.enableTexture == 0)
    {
        textureColor = gMaterial.color;
    }
    
    PixelShaderOutput output;
    
    //ライティングフラグが有効の時
    if (gMaterial.enableLighting)
    {
        float cos;
       
        //拡散反射を行うか否か
        if (gMaterial.enableHalfLambert)
        {     
            //Half Lambert処理でcosを計算
            float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
            cos = pow(NdotL * 0.5f + 0.5f, 1.0f);
            
            //追加で鏡面反射を適応させるか
            if (gMaterial.enablePhongReflection)
            {           
                //カメラへの方向算出
                float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
                
                //BlinnPhong反射モデルで鏡面反射を実装
                float32_t3 halfVector = normalize(-gDirectionalLight.direction + toEye);
                //
                float NDotH = dot(normalize(input.normal), halfVector);
                float specularPow = pow(saturate(NDotH), gMaterial.shininess);
                
                //拡散反射の色
                float32_t3 diffuce = input.color.rgb* gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
                //鏡面反射の色(反射色は白色を指定中)
                float32_t3 specular = input.color.rgb * gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
           
                //ポイントライトを含むか否か
                if (gMaterial.enablePointLight)
                {
                    //ライト方向
                    float32_t3 pointLightDirection = normalize(input.worldPosition - gPointLight.position);
                    
                    //ライトの色
                    float32_t3 pointlightColor = gPointLight.color.rgb * gPointLight.intensity;
                    
                    //ライトへの線w
                    float32_t3 direction = normalize(gPointLight.position - input.worldPosition);
                    
                    float32_t3 halfVector2 = normalize(-pointLightDirection + direction);
                
                    float NDot = dot(normalize(input.normal), halfVector2);
                    float specularPoww = pow(saturate(NDot), gMaterial.shininess);
                
                    //減衰係数の計算
                    float32_t distance = length(gPointLight.position - input.worldPosition);
                    float32_t factor = pow(saturate(-distance / gPointLight.radius + 1.0), gPointLight.decay);
                    
                     //拡散反射の色
                    float32_t3 diffucepoint = input.color.rgb * pointlightColor * textureColor.rgb * gPointLight.color.rgb * cos * gPointLight.intensity * factor;
                     //鏡面反射の色(反射色は白色を指定中)
                    float32_t3 specularpoint = input.color.rgb* pointlightColor * gPointLight.intensity * specularPoww * float32_t3(1.0f, 1.0f, 1.0f) * factor;
           
                   
                    //計算結果を合わせる
                    output.color.rgb = diffuce + specular + diffucepoint + specularpoint;
                    //output.color.rgb =  diffucepoint + specularpoint;
                
                 //アルファの処理
                    output.color.a = gMaterial.color.a * textureColor.a;
                }
                else
                {
                    //計算結果を合わせる
                    output.color.rgb = diffuce + specular;
                
                 //アルファの処理
                    output.color.a = gMaterial.color.a * textureColor.a;
                
                }
                
               
                 //textureのα値が0の時Pixelを棄却
                if (output.color.a <= gMaterial.discardNum)
                {
                    discard;
                }
                
                return output;
            }
        } //拡散処理を行う場合の処理終わり
        else
        {
            //Lambertian Reflectance
            cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        }
        //計算
        //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    
        output.color.rgb = input.color.rgb * gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
            //アルファの処理
        output.color.a = gMaterial.color.a * textureColor.a;
      
    } //シェーダーON時処理おわり
    else
    {
        output.color =input.color* gMaterial.color * textureColor;
    }
      
    
    //textureのα値が0の時Pixelを棄却
    if (output.color.a <= gMaterial.discardNum)
    {
        discard;
    }
    
    return output;
}