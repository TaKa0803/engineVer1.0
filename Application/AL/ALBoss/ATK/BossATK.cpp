#include "BossATK.h"
#include"AL/ALBoss/ALBoss.h"

BossATK::BossATK(ALBoss* boss)
{
	boss_ = boss;
	normal_ = std::make_unique<BossNormal>(boss);
}

void BossATK::SceneInit()
{
	modeType = Normal;
}

void BossATK::Initialize()
{
	//実際の初期化処理
	(this->*TypeInit[(int)modeType])();
}

void BossATK::Update()
{
	//状態の更新
	(this->*TypeUpdate[(int)modeType])();


}



void (BossATK::* BossATK::TypeInit[])() {
	&BossATK::InitNormal,
};
void (BossATK::* BossATK::TypeUpdate[])() {
	&BossATK::UpdateNormal
};


#pragma region 各状態の初期化と更新
void BossATK::InitNormal() { normal_->Initialize(); }
void BossATK::UpdateNormal() { normal_->Update(); }
#pragma endregion

