#include "BossNormalStump.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AL/ALBoss/ALBoss.h"

BossNormalStump::BossNormalStump(ALBoss* boss)
{
	boss_ = boss;
}

void BossNormalStump::Initialize()
{
	//エイム処理からスタート
	behaviReq_ =AIMing;

	isFinished_ = false;
}

void BossNormalStump::Update()
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

//関数群
void (BossNormalStump::* BossNormalStump::behaviorInit[])() {
	& BossNormalStump::InitAIMing,
	& BossNormalStump::InitWarning,
	& BossNormalStump::InitStump,
	& BossNormalStump::InitStiffness,
	& BossNormalStump::InitBack
};
//関数群
void (BossNormalStump::* BossNormalStump::behaviorUpdate[])() {
	& BossNormalStump::UpdateAIMing,
	& BossNormalStump::UpdateWarning,
	& BossNormalStump::UpdateStump,
	& BossNormalStump::UpdateStiffness,
	& BossNormalStump::UpdateBack

};

void BossNormalStump::InitAIMing()
{
	//使用する変数を初期化
	data_.currentCount = 0.0f;

	//開始地点を取得
	data_.aim.stPos = boss_->world_.translate_;
}

void BossNormalStump::InitWarning()
{
	data_.currentCount = 0.0f;
}

void BossNormalStump::InitStump()
{
	data_.currentCount = 0.0f;
}

void BossNormalStump::InitStiffness()
{
	data_.currentCount = 0.0f;
}

void BossNormalStump::InitBack()
{
	data_.currentCount = 0.0f;
}

void BossNormalStump::UpdateAIMing()
{
	AIMData& aim = data_.aim;
	//値の加算処理
	data_.currentCount += (float)DeltaTimer::deltaTime_;

	//高さの処理
	float hT = data_.currentCount  / aim.goTopSec;
	if (hT > 1.0f) {
		hT = 1.0f;
	}
	//高さを取得
	float height = Lerp(aim.stPos.y, boss_->GetPlayerWorldTranslate().y + aim.height, hT);
	
	//横移動の値
	float wT = data_.currentCount  / aim.xzSetSec;
	if (wT > 1.0f) {
		wT = 1.0f;
	}
	//横移動の値取得
	Vector3 pos = Lerp(aim.stPos, boss_->GetPlayerWorldTranslate(), wT);

	//先ほど求めた高さの値を代入
	pos.y = height;

	//値を合わせる
	boss_->world_.translate_ = pos;

	//シーン転換処理
	if (data_.currentCount >= aim.maxGoTop) {

		pos = boss_->GetPlayerWorldTranslate();
		pos.y += aim.height;
		boss_->world_.translate_ = pos;
		behaviReq_ = Warning;
	}
}

void BossNormalStump::UpdateWarning()
{
	//値の加算処理
	data_.currentCount += (float)DeltaTimer::deltaTime_;

	if (data_.currentCount >= data_.warning.maxWarning) {
		behaviReq_ = Stump;
	}
}

void BossNormalStump::UpdateStump()
{

	boss_->world_.translate_ += data_.stump.velo * data_.stump.spd*(float)DeltaTimer::deltaTime_;

	if (boss_->world_.translate_.y < 0) {
		boss_->world_.translate_.y = 0;

		behaviReq_ = Stiffness;
	}
}

void BossNormalStump::UpdateStiffness()
{
	//値の加算処理
	data_.currentCount += (float)DeltaTimer::deltaTime_;

	if (data_.currentCount >= data_.stiffnrss.stifnessSec) {
		behaviReq_ = Back;
	}

}

void BossNormalStump::UpdateBack()
{
	//値の加算処理
	data_.currentCount += (float)DeltaTimer::deltaTime_;

	if (data_.currentCount >= data_.back.maxBackSec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
}
