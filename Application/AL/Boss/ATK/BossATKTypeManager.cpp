#include "BossATKTypeManager.h"
#include"AL/Boss/Boss.h"
#include"GvariGroup/GvariGroup.h"


BossATKTypeManager::BossATKTypeManager(Boss* boss)
{
	boss_ = boss;
	normal_ = std::make_unique<BossNormalATKManager>(boss);

	//デバッグ設定
	std::unique_ptr<GVariGroup> gvg = std::make_unique<GVariGroup>("BossATKManager");
	gvg->SetMonitorValue("behavior", &nowTypeName_);
	gvg->SetTreeData(normal_->GetTree());

}

void BossATKTypeManager::SceneInit()
{
	modeType = Normal;
}

void BossATKTypeManager::Initialize()
{
	boss_->isFinishedATK_ = false;
	//実際の初期化処理
	(this->*TypeInit[(int)modeType])();
}

void BossATKTypeManager::Update()
{

#ifdef _DEBUG
	nowTypeName_ = typeName_[modeType];
#endif // _DEBUG

	//状態の更新
	(this->*TypeUpdate[(int)modeType])();

	//攻撃が終わった場合の処理
	//追い討ちなどかけない場合は素直にIDLE状態へ
	if (boss_->isFinishedATK_) {
		boss_->isFinishedATK_ = false;
		//とりあえずの命令終了
		boss_->SetBehavior(Boss::Behavior::IDLE);
	}
}



void (BossATKTypeManager::* BossATKTypeManager::TypeInit[])() {
	&BossATKTypeManager::InitNormal,
};
void (BossATKTypeManager::* BossATKTypeManager::TypeUpdate[])() {
	&BossATKTypeManager::UpdateNormal
};


#pragma region 各状態の初期化と更新
void BossATKTypeManager::InitNormal() { normal_->Initialize(); }
void BossATKTypeManager::UpdateNormal() { normal_->Update(); }
#pragma endregion

