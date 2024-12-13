#include"CopyImage.hlsli"

//頂点数
static const uint32_t kNumVertex = 3;
//三点の座標
static const float32_t4 kPositions[kNumVertex] =
{
    { -1.0f, 1.0f, 0.0f, 1.0f }, //左上
    { 3.0f, 1.0f, 0.0f, 1.0f }, //右上
    { -1.0f, -3.0f, 0.0f, 1.0f } //左下
};
//Texコード
static const float32_t2 kTexcoords[kNumVertex] =
{
    { 0.0f, 0.0f },
    { 2.0f, 0.0f },
    { 0.0f, 2.0f }
};


VertexShaderOutput main(uint32_t vertexId : SV_VertexID)
{
    VertexShaderOutput output;
    //画面を覆う三角形で描画
    output.position = kPositions[vertexId];
    output.texcoord = kTexcoords[vertexId];
    return output;
}