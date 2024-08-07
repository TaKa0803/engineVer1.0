
struct Particle
{
    int32_t isActive;
    float32_t3 translate;
    float32_t3 scale;
    float32_t  lifeTime;
    float32_t3 velocity;
    float32_t  currentTime;
    float32_t4 color;
};

struct PerView
{
    float32_t4x4 viewProjection;
    float32_t4x4 billboardMatrix;
    float32_t deltaTime;
};


RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

ConstantBuffer<PerView> gPerView : register(b0);

static const uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main( uint32_t3 DTid : SV_DispatchThreadID )
{
    uint32_t particleIndex = DTid.x;
    
    if (particleIndex < kMaxParticles)
    {
        //Activeではないものは処理しない
        if (gParticle[particleIndex].isActive!=0)
        {
            gParticle[particleIndex].translate += gParticle[particleIndex].velocity;
            gParticle[particleIndex].currentTime += gPerView.deltaTime;
            float32_t alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            gParticle[particleIndex].color.a = saturate(alpha);
            
            
            if (gParticle[particleIndex].color.a == 0)
            {
                //有効フラグOFFとスケールを０
                gParticle[particleIndex].isActive = 0;
                gParticle[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
                int32_t freeListIndex;
                InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
                
                
                
                //最新のFreeListIndexの場所に死んだParticleのIndexを設定する
                if ((freeListIndex + 1) < kMaxParticles)
                {
                    gFreeList[freeListIndex + 1] = particleIndex;
                }
                else
                {
                    //ここに来たら大問題なので対策
                    InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                }
            }
        }
    }
}