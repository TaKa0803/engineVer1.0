#include "EffectMove.h"
#include"TextureManager/TextureManager.h"

EffectMove::EffectMove()
{
	peM_ = std::make_unique<ParticleManager>();
}

EffectMove::~EffectMove()
{

}

void EffectMove::Initialize(const Vector4& color)
{
	peM_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png").texNum);
	peM_->SetOnlyImpact(true);
	EmiterSphere*emit=peM_->GetEmiterData();

	emit->color = color;
	emit->veloY.x = 0;
	emit->veloX = { -0.5f,0.5f };
	emit->veloZ = { -0.5f,0.5f };
	emit->speed = { 0.1f,0.3f };
	emit->lifetime = 0.1f;
	emit->count = { 1,1 };
}

void EffectMove::Update()
{
	peM_->Update();
}

void EffectMove::Draw()
{
	peM_->Draw();
}

void EffectMove::SpawnE(const Vector3& pos)
{
	peM_->SpawnE(pos);
}
