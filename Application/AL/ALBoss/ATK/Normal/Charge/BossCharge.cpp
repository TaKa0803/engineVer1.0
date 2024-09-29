#include "BossCharge.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AL/ALBoss/ALBoss.h"
#include<numbers>

BossCharge::BossCharge(ALBoss* boss)
{
	boss_ = boss;
}

void BossCharge::Initialize()
{
	behaviReq_ = AIMing;
	data_ = ATKData();
}

void BossCharge::Update()
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

void (BossCharge::* BossCharge::behaviorInit[])() = {
	&BossCharge::InitAIMing,
	&BossCharge::InitWarning,
	&BossCharge::InitDash,
	&BossCharge::InitStiffness,
	&BossCharge::InitBack
};

void (BossCharge::* BossCharge::behaviorUpdate[])() = {
	&BossCharge::UpdateAIMing,
	&BossCharge::UpdateWarning,
	&BossCharge::UpdateDash,
	&BossCharge::UpdateStiffness,
	&BossCharge::UpdateBack

};

void BossCharge::InitAIMing()
{
	data_.currentCount = 0;
}

void BossCharge::InitWarning()
{
	data_.currentCount = 0;
	//ダッシュ方向指定
	data_.dash.velo = Vector3{ boss_->GetPlayerWorldTranslate() - boss_->GetWorld().translate_ }.SetNormalize()*data_.dash.spd;
}

void BossCharge::InitDash()
{
	data_.currentCount = 0;
	data_.dash.prePos = boss_->world_.translate_;
	data_.dash.length = boss_->GetBoss2PlayerDirection().GetLength();
}

void BossCharge::InitStiffness()
{
	data_.currentCount = 0;
}

void BossCharge::InitBack()
{
	data_.currentCount = 0;
}

void BossCharge::UpdateAIMing()
{
	//プレイヤー方向を見続ける
	Vector3 direc = boss_->GetBoss2PlayerDirection();

	if (direc != Vector3(0, 0, 0)) {
		boss_->world_.rotate_.y = GetYRotate({direc.x,direc.z }) + ((float)std::numbers::pi);
	}


	data_.currentCount += (float)DeltaTimer::deltaTime_;
	if (data_.currentCount >= data_.aim.maxSec) {
		behaviReq_ = Warning;
	}
}

void BossCharge::UpdateWarning()
{
	data_.currentCount += (float)DeltaTimer::deltaTime_;
	if (data_.currentCount >= data_.warning.maxSec) {
		behaviReq_ = Dash;
	}
}

void BossCharge::UpdateDash()
{
	//プレイヤー方向にダッシュ
	boss_->world_.translate_ += data_.dash.velo*(float)DeltaTimer::deltaTime_;

	//過去位置からの距離取得
	float leng = Vector3{ data_.dash.prePos - boss_->GetWorld().GetWorldTranslate() }.GetLength();

	//最初に指定した距離以上で止まる処理
	if (leng >= data_.dash.length) {
		behaviReq_ = Stiffness;
	}

}

void BossCharge::UpdateStiffness()
{



	data_.currentCount += (float)DeltaTimer::deltaTime_;
	if (data_.currentCount >= data_.warning.maxSec) {
		boss_->isFinishedATK_ = true;
		behaviReq_ = Back;
	}
	else {
		float t = data_.currentCount / data_.stiffness.sec;
		Vector3 velo = Lerp(data_.dash.velo, { 0,0,0 }, t);

		//プレイヤー方向にダッシュ
		boss_->world_.translate_ += velo*(float)DeltaTimer::deltaTime_;

		data_.dash.velo = velo;
	}
}

void BossCharge::UpdateBack()
{
}
