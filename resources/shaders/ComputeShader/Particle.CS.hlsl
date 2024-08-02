
struct Particle
{
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
ConstantBuffer<PerView> gPerView : register(b0);

static const uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main( uint32_t3 DTid : SV_DispatchThreadID )
{
    uint32_t particleIndex = DTid.x;
    
    if (particleIndex < kMaxParticles)
    {
        //alphaが0の時はないものとみなして更新しない
        if (gParticle[particleIndex].color.a != 0)
        {
            //gParticle[particleIndex].translate += gParticle[particleIndex].velocity;
            //gParticle[particleIndex].currentTime = gPerView.deltaTime;
            //float32_t alpha = 1.0f - (gParticle[particleIndex].currentTime / gParticle[particleIndex].lifeTime);
            
        }
    }
}