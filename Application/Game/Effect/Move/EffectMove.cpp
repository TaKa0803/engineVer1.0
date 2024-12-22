#include "EffectMove.h"
#include"TextureManager/TextureManager.h"

EffectMove::EffectMove()
{
	//パーティクルクラス生成
	particleM_ = std::make_unique<ParticleManager>();
}

void EffectMove::Initialize(const Vector4& color)
{
	//パーてぅくるクラス初期化
	particleM_->Initialize(TextureManager::LoadTex("resources/Texture/CG/circle.png"));
	//関数呼び出し時のみ発生するフラグON
	particleM_->SetOnlyImpact(true);
	//パーティクルのエミッターパラメータ設定
	EmiterSphere*emit=particleM_->GetEmiterData();
	//各パラ設定
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
	//パーティクル更新
	particleM_->Update();
}

void EffectMove::Draw()
{
	//パーティクル描画
	particleM_->Draw();
}

void EffectMove::SpawnE(const Vector3& pos)
{
	//パーティクル出現
	particleM_->SpawnE(pos);
}
