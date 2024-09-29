#include "BossNormal.h"
#include"AL/ALBoss/ALBoss.h"
#include"RandomNum/RandomNum.h"

BossNormal::BossNormal(ALBoss* boss)
{
	boss_ = boss;

	stump_ = std::make_unique<BossNormalStump>(boss);
	charge_ = std::make_unique<BossCharge>(boss);
}

void BossNormal::Initialize()
{
	//ランダムな技を選択して使用
	//ここでtypeを変える処理

	if (RandomNumber::Get(0, 1) > 0.5f) {
		type_ = Stump;

	}
	else {
		type_ = Charge;

	}

	
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
	&BossNormal::InitStump,
	&BossNormal::InitShotBullet,
	& BossNormal::InitCharge
};

void (BossNormal::* BossNormal::typeUpdate[])() = {
	&BossNormal::UpdateStump,
	& BossNormal::UpdateShotBullet,
	& BossNormal::UpdateCharge
};

void BossNormal::InitStump() { stump_->Initialize(); }
void BossNormal::UpdateStump() { stump_->Update(); }

void BossNormal::InitShotBullet() {}
void BossNormal::UpdateShotBullet() {}

void BossNormal::InitCharge() { charge_->Initialize(); }
void BossNormal::UpdateCharge() { charge_->Update(); }
