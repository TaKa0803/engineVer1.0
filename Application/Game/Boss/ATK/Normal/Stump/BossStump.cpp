#include "BossStump.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game/Boss/Boss.h"


#pragma region 各状態初期化
BossStump::BossStump()
{
	treeData_.SetName("落下攻撃");

	treeData_.SetMonitorValue("経過時間", &currentCount_);

	treeData_.SetValue("構え時間", &data_.aim.maxGoTop);
	treeData_.SetValue("ジャンプ到達時間", &data_.warning.maxWarning);
	treeData_.SetValue("ジャンプ高度", &data_.warning.height);
	treeData_.SetValue("落下開始までの距離", &data_.warning.length);

	treeData_.SetValue("落下速度", &data_.stump.spd);

	treeData_.SetValue("落下硬直時間/s", &data_.stiffnrss.stifnessSec);
	treeData_.SetValue("硬直復帰時間/2", &data_.back.maxBackSec);
}
void BossStump::InitAIMing()
{
	boss_->SetAnimation((int)Boss::Animation::PreStump, 1, 1);
}

void BossStump::InitWarning()
{
	boss_->SetAnimation((int)Boss::Animation::Fly2Stump, 1, 1);
	data_.warning.stPos = boss_->world_.translate_;
}

void BossStump::InitATK()
{
	boss_->SetAnimeTime(false);
	boss_->SetAnimation((int)Boss::Animation::Stump, 0.0f, 1.0f,false);
	data_.stump.velo=boss_->GetBoss2PlayerDirection().GetNormalizeNum();
}

void BossStump::InitStiffness()
{
	boss_->SetAnimeTime(false);
	boss_->SetAnimation((int)Boss::Animation::EdStump, 0.0f, 2.0f,false);
}

void BossStump::InitBack()
{
	boss_->SetAnimation((int)Boss::Animation::Idle1, 1, 1);
}


#pragma endregion

#pragma region 各状態更新
void BossStump::UpdateAIMing()
{
	//狙う処理の変数取得
	AIMData& aim = data_.aim;

	////高さの処理
	//float hT = currentCount_ / aim.goTopSec;
	//if (hT > 1.0f) {
	//	hT = 1.0f;
	//}
	////高さを取得
	//float height = Lerp(aim.stPos.y, boss_->GetPlayerWorldTranslate().y + aim.height, hT);

	////横移動の値
	//float wT = currentCount_ / aim.xzSetSec;
	//if (wT > 1.0f) {
	//	wT = 1.0f;
	//}
	////横移動の値取得
	//Vector3 pos = Lerp(aim.stPos, boss_->GetPlayerWorldTranslate(), wT);

	////先ほど求めた高さの値を代入
	//pos.y = height;

	////値を合わせる
	//boss_->world_.translate_ = pos;

	//シーン転換処理
	if (currentCount_ >= aim.maxGoTop) {
		behaviReq_ = Warning;
	}
	else {

		float t = currentCount_ / aim.maxGoTop;
		boss_->SetAnimeTime(true, t);
	}
}

void BossStump::UpdateWarning()
{

	if (currentCount_ >= data_.warning.maxWarning) {
		behaviReq_ = ATK;
	}
	else {

		float t = currentCount_ / data_.warning.maxWarning;

		//プレイヤー方向にとびかかる
		//プレイヤー方向をみる
		boss_->SetDirection2Player();

		//とびかかり処理
		Vector3 bossP, playerP;
		bossP = boss_->GetWorld().GetWorldTranslate();
		playerP = boss_->GetPlayerWorldTranslate();

		//高さを無視する
		bossP.y = 0;
		playerP.y = 0;

		//プレイヤーからボス方向の向き取得
		Vector3 leng = Vector3(bossP - playerP).GetNormalizeNum();

		//長さから目標位置取得
		leng = playerP + leng;
		leng.y = data_.warning.height;

		//座標適応
		boss_->world_.translate_ = Lerp(data_.warning.stPos, leng, t);
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

	if (currentCount_ >= data_.stiffnrss.stifnessSec) {
		behaviReq_ = Back;
	}
}

void BossStump::UpdateBack()
{

	if (currentCount_ >= data_.back.maxBackSec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
}
#pragma endregion



