
struct Well
{
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

struct Vertex
{
    float32_t4 position;
    float32_t2 texcoord;
    float32_t3 normal;
};

struct VertexInflluence
{
    float32_t4 weight;
    int32_t4 index;
};

struct SkinningInformation
{
    uint32_t numVertices;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);

StructuredBuffer<Vertex> gInputVertices : register(t1);

StructuredBuffer<VertexInflluence> gInfluences : register(t2);

//スキニング計算後の頂点データ
RWStructuredBuffer<Vertex> gOutputVertices : register(u0);

//Skinningに関する情報
ConstantBuffer<SkinningInformation> gSkinningInformation : register(b0);


[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint32_t vertexIndex = DTid.x;
    if (vertexIndex < gSkinningInformation.numVertices)
    {
        //Skinning計算
        //必要なデータをStructuredBufferからとってくる
        //SkinningObject3d.VSでは入力頂点として受け取っていた
        Vertex input = gInputVertices[vertexIndex];
        VertexInflluence influence = gInfluences[vertexIndex];
        
        Vertex skinned;
        skinned.texcoord = input.texcoord;
        
        //計算方法はSkinningObjectと同じ
        //dataの取得方法が変わるだけで、原理が変わる訳ではない
        skinned.position = mul(input.position, gMatrixPalette[influence.index.x].skeletonSpaceMatrix) * influence.weight.x;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.y].skeletonSpaceMatrix) * influence.weight.y;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.z].skeletonSpaceMatrix) * influence.weight.z;
        skinned.position += mul(input.position, gMatrixPalette[influence.index.w].skeletonSpaceMatrix) * influence.weight.w;
        skinned.position.w = 1.0f; //確実に1を入れる
    
        
        skinned.normal = mul(input.normal, (float32_t3x3) gMatrixPalette[influence.index.x].skeletonSpaceInverseTransposeMatrix) * influence.weight.x;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[influence.index.y].skeletonSpaceInverseTransposeMatrix) * influence.weight.y;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[influence.index.z].skeletonSpaceInverseTransposeMatrix) * influence.weight.z;
        skinned.normal += mul(input.normal, (float32_t3x3) gMatrixPalette[influence.index.w].skeletonSpaceInverseTransposeMatrix) * influence.weight.w;
        skinned.normal = normalize(skinned.normal);
    
        gOutputVertices[vertexIndex] = skinned;
        
        
    }
    
}