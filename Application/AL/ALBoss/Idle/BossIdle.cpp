#include "BossIdle.h"
#include"AL/ALBoss/ALBoss.h"
#include"DeltaTimer/DeltaTimer.h"
#include"RandomNum/RandomNum.h"

BossIdle::BossIdle(ALBoss* boss)
{
	boss_ = boss;
}

void BossIdle::Initialize()
{
	//待機時間カウントリセット
	data_.currentSec = 0;

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
			boss_->behaviorReq_ = ALBoss::Behavior::MOVE;
		}
		else {
			boss_->behaviorReq_ = ALBoss::Behavior::ATK;
			//boss_->behaviorReq_ = ALBoss::Behavior::MOVE;
		}
		
	}


}