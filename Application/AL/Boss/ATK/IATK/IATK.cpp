#include "IATK.h"

bool IATK::isEnd_ = false;

Boss* IATK::boss_ = nullptr;

void IATK::InitIATK()
{



	//最初の状態に変更
	behaviReq_ = AIMing;
	//カウントをゼロに変更
	currentCount_ = 0;

	isEnd_ = false;
}

void IATK::BehaviorUpdating()
{
	//状態の初期化処理
	if (behaviReq_) {
		behavior_ = behaviReq_.value();
		behaviReq_ = std::nullopt;
		//実際の初期化処理
		(this->*behaviorInit[(int)behavior_])();
	}

	//状態の更新
	(this->*behaviorUpdate[(int)behavior_])();
}

void IATK::SetBossPointer(Boss* boss)
{
	boss_ = boss;
}

void IATK::Init()
{
	//共通パラ初期化
	InitIATK();
}

void IATK::Update()
{
	//更新
	BehaviorUpdating();
}

void (IATK::* IATK::behaviorInit[])() {
	& IATK::InitAIMing,
	& IATK::InitWarning,
	& IATK::InitATK,
	& IATK::InitStiffness,
	& IATK::InitBack
};

void(IATK::* IATK::behaviorUpdate[])() {
	& IATK::UpdateAIMing,
	& IATK::UpdateWarning,
	& IATK::UpdateATK,
	& IATK::UpdateStiffness,
	& IATK::UpdateBack
};