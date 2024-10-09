#include "BossStump.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AL/Boss/Boss.h"


#pragma region 各状態初期化
void BossStump::InitAIMing()
{
	//使用する変数を初期化
	currentCount_ = 0.0f;

	//開始地点を取得
	data_.aim.stPos = boss_->world_.translate_;
}

void BossStump::InitWarning()
{
	currentCount_ = 0.0f;
}

void BossStump::InitATK()
{
	currentCount_ = 0.0f;
}

void BossStump::InitStiffness()
{
	currentCount_ = 0.0f;
}

void BossStump::InitBack()
{
	currentCount_ = 0.0f;
}
#pragma endregion

#pragma region 各状態更新
void BossStump::UpdateAIMing()
{
	//狙う処理の変数取得
	AIMData& aim = data_.aim;
	//値の加算処理
	currentCount_ += (float)DeltaTimer::deltaTime_;

	//高さの処理
	float hT = currentCount_ / aim.goTopSec;
	if (hT > 1.0f) {
		hT = 1.0f;
	}
	//高さを取得
	float height = Lerp(aim.stPos.y, boss_->GetPlayerWorldTranslate().y + aim.height, hT);

	//横移動の値
	float wT = currentCount_ / aim.xzSetSec;
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
	if (currentCount_ >= aim.maxGoTop) {

		pos = boss_->GetPlayerWorldTranslate();
		pos.y += aim.height;
		boss_->world_.translate_ = pos;
		behaviReq_ = Warning;
	}
}

void BossStump::UpdateWarning()
{
	//値の加算処理
	currentCount_ += (float)DeltaTimer::deltaTime_;

	if (currentCount_ >= data_.warning.maxWarning) {
		behaviReq_ = ATK;
	}
}

void BossStump::UpdateATK()
{
	boss_->world_.translate_ += data_.stump.velo * data_.stump.spd * (float)DeltaTimer::deltaTime_;

	if (boss_->world_.translate_.y < 0) {
		boss_->world_.translate_.y = 0;

		behaviReq_ = Stiffness;
	}
}

void BossStump::UpdateStiffness()
{
	//値の加算処理
	currentCount_ += (float)DeltaTimer::deltaTime_;

	if (currentCount_ >= data_.stiffnrss.stifnessSec) {
		behaviReq_ = Back;
	}
}

void BossStump::UpdateBack()
{
	//値の加算処理
	currentCount_ += (float)DeltaTimer::deltaTime_;

	if (currentCount_ >= data_.back.maxBackSec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
}
#pragma endregion



