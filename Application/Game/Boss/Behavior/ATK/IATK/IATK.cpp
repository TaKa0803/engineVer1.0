#include "IATK.h"
#include"DeltaTimer/DeltaTimer.h"

bool IBossATK::isEnd_ = false;

Boss* IBossATK::boss_ = nullptr;

void IBossATK::InitIATK()
{

	//最初の状態に変更
	behaviReq_ = AIMing;
	//カウントをゼロに変更
	currentCount_ = 0;

	isEnd_ = false;
}

void IBossATK::UpdateBehavior()
{

#ifdef _DEBUG
	nowBehavior_ = behaviorName_[behavior_];
#endif // _DEBUG


	//状態の初期化処理
	if (behaviReq_) {
		behavior_ = behaviReq_.value();
		behaviReq_ = std::nullopt;
		//実際の初期化処理
		(this->*behaviorInit[(int)behavior_])();

		//カウント初期化
		currentCount_ = 0;
	}

	//カウント増加
	currentCount_ += (float)DeltaTimer::deltaTime_;

	//状態の更新
	(this->*behaviorUpdate[(int)behavior_])();
}

IBossATK::IBossATK()
{
	//デバッグ設定
	//treeData_.SetMonitorValue("状態", &nowBehavior_);
}

void IBossATK::SetBossPointer(Boss* boss)
{
	boss_ = boss;
}

void IBossATK::Init()
{
	//共通パラ初期化
	InitIATK();
}

void IBossATK::Update()
{
	//更新
	UpdateBehavior();
}

void IBossATK::SetParam2Tree(const std::string& treename)
{
	treeData_.name_ = treename;
	treeData_.SetMonitorValue("カウント", &currentCount_);

	treeData_.SetValue("AIM状態時間", &parameters_.aimingSec);
	treeData_.SetValue("実攻撃前状態時間", &parameters_.warningSec);
	treeData_.SetValue("実攻撃状態時間", &parameters_.ATKSec);
	treeData_.SetValue("スタン状態時間", &parameters_.stiffnessSec);
	treeData_.SetValue("通常に戻る状態時間", &parameters_.backSec);

}

void (IBossATK::* IBossATK::behaviorInit[])() {
	& IBossATK::InitAIMing,
	& IBossATK::InitWarning,
	& IBossATK::InitATK,
	& IBossATK::InitStiffness,
	& IBossATK::InitBack
};

void(IBossATK::* IBossATK::behaviorUpdate[])() {
	& IBossATK::UpdateAIMing,
	& IBossATK::UpdateWarning,
	& IBossATK::UpdateATK,
	& IBossATK::UpdateStiffness,
	& IBossATK::UpdateBack
};