#include "BossIdle.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"


BossIdle::BossIdle(Boss* boss)
{
	boss_ = boss;
	
	tree_.name_ = "idle";
	tree_.SetValue("sec", &data_.idleSec);
	tree_.SetMonitorValue("current sec", &data_.currentSec);
	
}

void BossIdle::Initialize()
{
	//待機時間カウントリセット
	data_.currentSec = 0;

	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], 0, 1);
}

void BossIdle::Update()
{

	//待機終わったら処理
	data_.currentSec += (float)DeltaTimer::deltaTime_;
	if (data_.currentSec >= data_.idleSec) {
		//距離によって動き
		float rand = RandomNumber::Get(0.0f, 1.0f);

		//ランダムで攻撃か移動か変更
		if (rand < 0.5f) {
			boss_->SetBehavior(Boss::Behavior::DOWN);
		}
		else {
			boss_->SetBehavior(Boss::Behavior::ATK);
		}
		
		boss_->SetBehavior(Boss::Behavior::ATK);
	}
	else {
		float t = data_.currentSec / data_.idleSec;

		boss_->SetAnimeTime(true,t);
	}


}

