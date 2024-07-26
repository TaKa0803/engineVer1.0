
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
};


StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b0);

static const uint32_t kMaxParticles = 1024;

[numthreads(1024, 1, 1)]
void main( uint32_t3 DTid : SV_DispatchThreadID )
{

}