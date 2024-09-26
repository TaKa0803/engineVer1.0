#include "BossNormal.h"
#include"AL/ALBoss/ALBoss.h"

BossNormal::BossNormal(ALBoss* boss)
{
	boss_ = boss;

	stump_ = std::make_unique<BossNormalStump>(boss);
}

void BossNormal::Initialize()
{
	//ランダムな技を選択して使用
	//ここでtypeを変える処理

	//初期化
	(this->*typeInit[type_])();

	isFinished_ = false;
}

void BossNormal::Update()
{
	//更新処理
	(this->*typeUpdate[type_])();

	//攻撃が終わった場合の処理
	//追い討ちなどかけない場合は素直にIDLE状態へ
	if (boss_->isFinishedATK_) {
		boss_->isFinishedATK_ = false;
		//とりあえずの命令終了

		boss_->behaviorReq_ = ALBoss::IDLE;
	}
}



void (BossNormal::* BossNormal::typeInit[])() = {
	&BossNormal::InitStump
};

void (BossNormal::* BossNormal::typeUpdate[])() = {
	&BossNormal::UpdateStump
};

void BossNormal::InitStump() { stump_->Initialize(); }
void BossNormal::UpdateStump() { stump_->Update(); }

void BossNormal::InitShotBullet() {}
void BossNormal::UpdateShotBullet() {}

void BossNormal::InitCharge() {}
void BossNormal::UpdateCharge() {}
