#include "BossPunch.h"
#include"Game/Boss/Boss.h"

BossPunch::BossPunch()
{
	//パラメータ変数をツリーに追加
	SetParam2Tree("パンチ攻撃");
}

void BossPunch::InitAIMing()
{
	//プレイヤー方向を向く
	boss_->SetDirection2Player();
	//アニメーションを変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PrePunch], parameters_.aimingSec,1,false);
}

void BossPunch::InitATK()
{
	//アニメーションを変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Punch], parameters_.ATKSec, 1,false);
	//攻撃コライダーを有効
	boss_->GetATKCollider().isActive_ = true;
}

void BossPunch::InitBack()
{
	//アニメーション変更
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], parameters_.backSec, 1,false);
	//攻撃コライダー無効化
	boss_->GetATKCollider().isActive_ = false;
}


void BossPunch::UpdateAIMing()
{
	//時間取得
	float sec = parameters_.aimingSec;

	//カウント達成で処理
	if (currentCount_ >= sec) {
		//攻撃状態に
		behaviReq_ = ATK;
	}
	else {
		//割合取得
		float t = currentCount_ / sec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossPunch::UpdateATK()
{
	//時間取得
	float sec = parameters_.ATKSec;

	//カウント達成で処理
	if (currentCount_ >= sec) {
		//状態を変更
		behaviReq_ = Back;
	}
	else {
		//割合取得
		float t = currentCount_ / sec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossPunch::UpdateBack()
{
	//時間取得
	float sec = parameters_.backSec;

	//カウント達成で処理
	if (currentCount_ >= sec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
	else {
		//割合取得
		float t = currentCount_ / sec;
		//アニメーション再生をTで管理
		boss_->SetAnimeTime(true, t);
	}
}
