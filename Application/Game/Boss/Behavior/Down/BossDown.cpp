#include"BossDown.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossDown::BossDown(Boss*boss)
{
	boss_ = boss;

	tree_.name_ = "ダウン";
	tree_.SetValue("ダウンアニメーション", &animeSec_);
	tree_.SetValue("次に行くまでの時間", &nextSec_);

}

void BossDown::Initialize() {
	//アニメーションセット
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Down], animeSec_, animeSec_, false);
	currentNext_ = 0;
}

void BossDown::Update() {
	currentNext_ += (float)DeltaTimer::deltaTime_;

	//カウント以上で返却
	if (currentNext_ >= nextSec_) {
		boss_->isDead_ = true;
	}
}