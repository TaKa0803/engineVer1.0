#include "BossSumerSolt.h"
#include"Game/Boss/Boss.h"

BossSumerSolt::BossSumerSolt()
{
	//パラメータをツリーに追加
	SetParam2Tree("サマーソルト");
}

void BossSumerSolt::InitAIMing()
{
	//プレイヤー方向を向く
	boss_->SetDirection2Player();
	//アニメーション設定
 	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PreSumersolt], 0, 1);
}

void BossSumerSolt::InitATK()
{
	//攻撃コライダー有効化
	boss_->GetATKCollider().isActive_ = true;
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::SumerSolt], 0, 1);
}

void BossSumerSolt::InitBack()
{
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1],0, 1);
	//攻撃コライダー無効化
	boss_->GetATKCollider().isActive_ = false;
}

void BossSumerSolt::UpdateAIMing()
{
	//秒数取得
	float sec = parameters_.aimingSec;

	//カウント達成で処理
	if (currentCount_ >= sec) {
		//攻撃状態へ
		behaviReq_ = ATK;
	}
	else {
		//割合取得
		float t = currentCount_ / sec;
		//アニメーション進行をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossSumerSolt::UpdateATK()
{
	//時間取得
	float sec = parameters_.ATKSec;

	//カウント達成で処理
	if (currentCount_ >= sec) {
		//終了処理状態へ移行
		behaviReq_ = Back;
	}
	else {
		//割合取得
		float t = currentCount_ / sec;
		//アニメーション進行をTで管理
		boss_->SetAnimeTime(true, t);
	}
}

void BossSumerSolt::UpdateBack()
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
		//アニメーション進行をTで管理
		boss_->SetAnimeTime(true, t);
	}
}
