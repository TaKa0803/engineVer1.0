#include "BossCharge.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game//Boss/Boss.h"
#include<numbers>

BossCharge::BossCharge()
{
	treeData_.SetName("タックル");
	treeData_.SetValue("狙う時間/s", &data_.aim.maxSec);
	treeData_.SetValue("警告時間/s", &data_.warning.maxSec);
	treeData_.SetValue("ダッシュ速度", &data_.dash.spd);

	treeData_.SetValue("硬直時間/s", &data_.stiffness.sec);
	treeData_.SetValue("復帰時間/s", &data_.back.sec);
}

void BossCharge::InitAIMing() {
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PreCharge], 0.2f, 1, false);
}

void BossCharge::InitWarning()
{
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Charging], 0.2f, 1,false);
	//ダッシュ方向指定
	data_.dash.velo = Vector3{ boss_->GetPlayerWorldTranslate() - boss_->GetWorld().translate_ }.SetNormalize() * data_.dash.spd;
}

void BossCharge::InitATK()
{
	boss_->GetATKCollider().isActive_ = true;
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Charging], 0.2f, 0.5f, true);
	boss_->SetAnimeTime(false);

	data_.dash.prePos = boss_->world_.translate_;
	data_.dash.length = boss_->GetBoss2PlayerDirection().GetLength();
}

void BossCharge::InitStiffness() {
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::EdCharge], 0.2f, 1, false);

}

void BossCharge::InitBack() {
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], data_.back.sec, 1, false);
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
	else {
		float t = currentCount_ / data_.aim.maxSec;
		boss_->SetAnimeTime(true, t);
	}
}

void BossCharge::UpdateWarning()
{

	if (currentCount_ >= data_.warning.maxSec) {
		behaviReq_ = ATK;
	}
	else {
		float t = currentCount_ / data_.warning.maxSec;
		boss_->SetAnimeTime(true, t);
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
		behaviReq_ = Back;
	}
	else {
		float t = currentCount_ / data_.warning.maxSec;
		Vector3 velo = Lerp(data_.dash.velo, { 0,0,0 }, t);

		//ベクトル方向に移動
		boss_->world_.translate_ += velo * (float)DeltaTimer::deltaTime_;

		data_.dash.velo = velo;

		boss_->SetAnimeTime(true, t);
	
	}
}

void BossCharge::UpdateBack() {

	if (currentCount_ >= data_.back.sec) {
		boss_->isFinishedATK_ = true;
	}
	else {
		float t = currentCount_ / data_.warning.maxSec;
		boss_->SetAnimeTime(true, t);
	}

}
