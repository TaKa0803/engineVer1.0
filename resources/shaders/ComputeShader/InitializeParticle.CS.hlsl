struct Particle
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};
RWStructuredBuffer<Particle> gParticle : register(u0);

static const uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    if (particleIndex < kMaxParticles)
    {
        //構造体の全要素を０で埋める
        gParticle[particleIndex] = (Particle) 0;
        
        gParticle[particleIndex].scale = float32_t3(1.0f, 1.0f, 1.0f);
        gParticle[particleIndex].color = float32_t4(1.0f, 1.0f, 1.0f, 1.0f);

    }
}