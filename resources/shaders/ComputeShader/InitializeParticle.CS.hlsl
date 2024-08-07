struct Particle
{
    int32_t isActive;
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};
RWStructuredBuffer<Particle> gParticle : register(u0);

RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

static const uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        //構造体の全要素を０で埋める
        gParticle[particleIndex] = (Particle) 0;
        
        gFreeList[particleIndex] = particleIndex;
        
    }
    
    if (particleIndex == 0)
    {
        //末尾を指すように
        gFreeListIndex[0] = kMaxParticles - 1;
    }
}