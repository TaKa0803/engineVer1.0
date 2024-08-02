
struct Particle
{
    float32_t3 translate;
    float32_t3 scale;
    float32_t lifeTime;
    float32_t3 velocity;
    float32_t currentTime;
    float32_t4 color;
};

struct Emiter
{
    float32_t3 translate;
    float32_t radius;
    uint32_t count;
    float32_t frequency;
    float32_t frequencyTime;
    uint32_t emit;
};

struct PerFrame
{
    float32_t time;
    float32_t deltaTime;
};

RWStructuredBuffer<Particle> gParticle : register(u0);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);
ConstantBuffer<Emiter> gEmiter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719))
{
    //make value smaller to avoid artefacts
    float32_t3 smallValue = sin(value);
    //get scalar value from 3d vector
    float32_t random = dot(smallValue, dotDir);
    //make value more random by making it bigger and then taking teh factional part
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t3 rand3dTo3d(float32_t3 value)
{
    return float32_t3(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float32_t3(73.156, 52.235, 09.151))
    );
}

class RandomGenerator
{
    float32_t3 seed;
    float32_t3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t Generate1d()
    {
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};

static const uint32_t kMaxParticles = 1024;

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    
    
    if (gEmiter.emit != 0)
    {
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        for (uint32_t countIndex = 0; countIndex < gEmiter.count; ++countIndex)
        {
            uint32_t particleIndex;
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            
            //
            if (particleIndex < kMaxParticles)
            {
                float32_t scale = generator.Generate1d();
                
                gParticle[particleIndex].scale = float32_t3(scale, scale, scale);
                gParticle[particleIndex].translate = generator.Generate3d() * (gEmiter.radius - gEmiter.radius / 2.0f);
                gParticle[particleIndex].velocity = generator.Generate3d() * 2 - 1;
                gParticle[particleIndex].color.rgb = generator.Generate3d();
                gParticle[particleIndex].color.a = 1.0f;
            }
        }

    }
}