#include "BossPunch.h"
#include"Game/Boss/Boss.h"

BossPunch::BossPunch()
{
	SetParam2Tree("パンチ攻撃");


}

void BossPunch::InitAIMing()
{
	boss_->SetDirection2Player();
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PrePunch], 1, 1);
}

void BossPunch::InitWarning()
{
}

void BossPunch::InitATK()
{
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Punch], 1, 1);
	boss_->GetATKCollider().isActive_ = true;
}

void BossPunch::InitStiffness()
{
}

void BossPunch::InitBack()
{
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], 1, 1);
	boss_->GetATKCollider().isActive_ = true;
}


void BossPunch::UpdateAIMing()
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

void BossPunch::UpdateWarning()
{
}

void BossPunch::UpdateATK()
{
	float sec = parameters_.ATKSec;

	if (currentCount_ >= sec) {
		behaviReq_ = Back;
	}
	else {
		float t = currentCount_ / sec;
		boss_->SetAnimeTime(true, t);
	}
}

void BossPunch::UpdateStiffness()
{
}

void BossPunch::UpdateBack()
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
