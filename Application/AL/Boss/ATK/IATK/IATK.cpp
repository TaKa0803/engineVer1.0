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