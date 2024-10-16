#include "BossCharge.h"
#include"DeltaTimer/DeltaTimer.h"
#include"AL//Boss/Boss.h"
#include<numbers>



void BossCharge::InitWarning()
{
	//ダッシュ方向指定
	data_.dash.velo = Vector3{ boss_->GetPlayerWorldTranslate() - boss_->GetWorld().translate_ }.SetNormalize() * data_.dash.spd;
}

void BossCharge::InitATK()
{
	data_.dash.prePos = boss_->world_.translate_;
	data_.dash.length = boss_->GetBoss2PlayerDirection().GetLength();
}



void BossCharge::UpdateAIMing()
{
	//プレイヤー方向を見続ける
	Vector3 direc = boss_->GetBoss2PlayerDirection();

	if (direc != Vector3(0, 0, 0)) {
		boss_->world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}

	if (currentCount_ >= data_.aim.maxSec) {
		behaviReq_ = Warning;
	}
}

void BossCharge::UpdateWarning()
{

	if (currentCount_ >= data_.warning.maxSec) {
		behaviReq_ = ATK;
	}
}

void BossCharge::UpdateATK()
{
	//プレイヤー方向にダッシュ
	boss_->world_.translate_ += data_.dash.velo * (float)DeltaTimer::deltaTime_;

	//過去位置からの距離取得
	float leng = Vector3{ data_.dash.prePos - boss_->GetWorld().GetWorldTranslate() }.GetLength();

	//最初に指定した距離以上で止まる処理
	if (leng >= data_.dash.length) {
		behaviReq_ = Stiffness;
	}
}

void BossCharge::UpdateStiffness()
{

	if (currentCount_ >= data_.warning.maxSec) {
		boss_->isFinishedATK_ = true;
	}
	else {
		float t = currentCount_ / data_.stiffness.sec;
		Vector3 velo = Lerp(data_.dash.velo, { 0,0,0 }, t);

		//プレイヤー方向にダッシュ
		boss_->world_.translate_ += velo * (float)DeltaTimer::deltaTime_;

		data_.dash.velo = velo;
	}
}

