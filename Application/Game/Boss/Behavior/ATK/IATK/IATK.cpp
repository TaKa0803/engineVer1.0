#include "IATK.h"
#include"DeltaTimer/DeltaTimer.h"

//ボスポインタ初期化
Boss* BossBaseATK::boss_ = nullptr;

void BossBaseATK::InitIATK()
{
	//最初の状態に変更
	behaviReq_ = AIMing;
	//カウントをゼロに変更
	currentCount_ = 0;
}

void BossBaseATK::UpdateBehavior()
{
#ifdef _DEBUG
	//現在の状態の名前に変更
	nowBehavior_ = behaviorName_[behavior_];
#endif // _DEBUG

	//状態の初期化処理
	if (behaviReq_) {
		//リクエストの値を渡す
		behavior_ = behaviReq_.value();
		//リクエストを空に
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

BossBaseATK::BossBaseATK()
{
	//デバッグ設定
	//treeData_.SetMonitorValue("状態", &nowBehavior_);
}

void BossBaseATK::SetBossPointer(Boss* boss)
{
	//ボスのポインタ設定
	boss_ = boss;
}

void BossBaseATK::Initialize()
{
	//共通パラ初期化
	InitIATK();
}

void BossBaseATK::Update()
{
	//更新
	UpdateBehavior();
}

void BossBaseATK::SetParam2Tree(const std::string& treename)
{
	//デバッグ用にツリーにパラメータ追加
	treeData_.name_ = treename;
	treeData_.SetMonitorValue("カウント", &currentCount_);
	treeData_.SetValue("AIM状態時間", &parameters_.aimingSec);
	treeData_.SetValue("実攻撃前状態時間", &parameters_.warningSec);
	treeData_.SetValue("実攻撃状態時間", &parameters_.ATKSec);
	treeData_.SetValue("スタン状態時間", &parameters_.stiffnessSec);
	treeData_.SetValue("通常に戻る状態時間", &parameters_.backSec);

}

//初期化関数をまとめる
void (BossBaseATK::* BossBaseATK::behaviorInit[])() {
	& BossBaseATK::InitAIMing,
	& BossBaseATK::InitWarning,
	& BossBaseATK::InitATK,
	& BossBaseATK::InitStiffness,
	& BossBaseATK::InitBack
};

//更新処理関数をまとめる
void(BossBaseATK::* BossBaseATK::behaviorUpdate[])() {
	& BossBaseATK::UpdateAIMing,
	& BossBaseATK::UpdateWarning,
	& BossBaseATK::UpdateATK,
	& BossBaseATK::UpdateStiffness,
	& BossBaseATK::UpdateBack
};