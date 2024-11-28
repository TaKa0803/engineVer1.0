#include "BossSumerSolt.h"
#include"Game/Boss/Boss.h"

BossSumerSolt::BossSumerSolt()
{
	SetParam2Tree("サマーソルト");
}

void BossSumerSolt::InitAIMing()
{
	boss_->SetDirection2Player();
 	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PreSumersolt], 0, 1);
}

void BossSumerSolt::InitWarning()
{
}

void BossSumerSolt::InitATK()
{
	boss_->SetAnimeTime(false);
	boss_->GetATKCollider().isActive_ = true;
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::SumerSolt], parameters_.ATKSec, 1);
}

void BossSumerSolt::InitStiffness()
{
}

void BossSumerSolt::InitBack()
{
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1],0, 1);
	boss_->GetATKCollider().isActive_ = true;
}

void BossSumerSolt::UpdateAIMing()
{
	float sec = parameters_.aimingSec;

	if (currentCount_ >= sec) {
		behaviReq_ = ATK;
	}
	else {
		float t = currentCount_ / sec;
		boss_->SetAnimeTime(true, t);
	}
}

void BossSumerSolt::UpdateWarning()
{
}

void BossSumerSolt::UpdateATK()
{
	float sec = parameters_.ATKSec;

	if (currentCount_ >= sec) {
		behaviReq_ = Back;
	}
	else {
		float t = currentCount_ / sec;
		//boss_->SetAnimeTime(true, t);
	}
}

void BossSumerSolt::UpdateStiffness()
{
}

void BossSumerSolt::UpdateBack()
{
	float sec = parameters_.backSec;

	if (currentCount_ >= sec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
	else {
		float t = currentCount_ / sec;
		boss_->SetAnimeTime(true, t);
	}
}
