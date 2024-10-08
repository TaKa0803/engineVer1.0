#include "BossATK.h"
#include"AL/Boss/Boss.h"

BossATKManager::BossATKManager(Boss* boss)
{
	boss_ = boss;
	normal_ = std::make_unique<BossNormalATKManager>(boss);
}

void BossATKManager::SceneInit()
{
	modeType = Normal;
}

void BossATKManager::Initialize()
{
	boss_->isFinishedATK_ = false;
	//実際の初期化処理
	(this->*TypeInit[(int)modeType])();
}

void BossATKManager::Update()
{
	//状態の更新
	(this->*TypeUpdate[(int)modeType])();

	//攻撃が終わった場合の処理
	//追い討ちなどかけない場合は素直にIDLE状態へ
	if (boss_->isFinishedATK_) {
		boss_->isFinishedATK_ = false;
		//とりあえずの命令終了

		boss_->behaviorReq_ = Boss::IDLE;
	}
}



void (BossATKManager::* BossATKManager::TypeInit[])() {
	&BossATKManager::InitNormal,
};
void (BossATKManager::* BossATKManager::TypeUpdate[])() {
	&BossATKManager::UpdateNormal
};


#pragma region 各状態の初期化と更新
void BossATKManager::InitNormal() { normal_->Initialize(); }
void BossATKManager::UpdateNormal() { normal_->Update(); }
#pragma endregion

