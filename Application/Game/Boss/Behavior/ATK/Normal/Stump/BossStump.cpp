#include "BossStump.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game/Boss/Boss.h"


#pragma region 各状態初期化
BossStump::BossStump()
{
	//ツリーにデバッグ用パラメータを設定
	treeData_.name_ = "落下攻撃";
	treeData_.SetValue("構え時間", &parameters_.aimingSec);
	treeData_.SetValue("ジャンプ到達時間", &parameters_.warningSec);
	treeData_.SetValue("ジャンプ高度", &data_.warning.height);
	treeData_.SetValue("落下開始までの距離", &data_.warning.length);
	treeData_.SetValue("落下速度", &data_.stump.spd);
	treeData_.SetValue("落下アニメーション速度倍率", &data_.stump.animeMultiply);
	treeData_.SetValue("落下硬直時間/s", &parameters_.stiffnessSec);
	treeData_.SetValue("硬直復帰時間/2", &parameters_.backSec);
}
void BossStump::InitAIMing()
{
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::PreStump], parameters_.aimingSec, 1,false);
}

void BossStump::InitWarning()
{
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Fly2Stump], parameters_.warningSec, 1);
	//開始時の座標保存
	data_.warning.stPos = boss_->world_.translate_;
}

void BossStump::InitATK()
{
	//アニメーション再生をこちらから管理する処理OFF
	boss_->SetAnimeTime(false);
	//アニメーション設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Stump],0.0f, data_.stump.animeMultiply,false);
	//落下方向設定
	data_.stump.velo=boss_->GetBoss2PlayerDirection().GetNormalizeNum();
	//攻撃コライダーを有効化
	boss_->GetATKCollider().isActive_ = true;
}

void BossStump::InitStiffness()
{
	//攻撃コライダーを無効化
	boss_->GetATKCollider().isActive_ = true;
	//アニメーションを設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::EdStump], parameters_.stiffnessSec, 1.0f,false);
}

void BossStump::InitBack()
{
	//アニメーションを設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle1], 1, 1);
}


#pragma endregion

#pragma region 各状態更新
void BossStump::UpdateAIMing()
{
	//カウント達成で遷移
	if (currentCount_ >= parameters_.aimingSec) {
		//状態を警告に
		behaviReq_ = Warning;
	}
	else {
		//割合取得
		float t = currentCount_ / parameters_.aimingSec;
		//アニメーション再生をTで処理
		boss_->SetAnimeTime(true, t);
	}
}

void BossStump::UpdateWarning()
{
	//カウント達成で次の処理へ
	if (currentCount_ >= parameters_.warningSec) {
		//攻撃状態へ
		behaviReq_ = ATK;
	}
	else {
		//割合取得
		float t = currentCount_ / parameters_.warningSec;

		//プレイヤー方向をみる
		boss_->SetDirection2Player();

		//とびかかり処理
		//プレイヤーとボスの座標取得
		Vector3 bossP, playerP;
		bossP = boss_->world_.GetWorldTranslate();
		playerP = boss_->GetPlayerWorldTranslate();

		//高さを無視する
		bossP.y = 0;
		playerP.y = 0;

		//プレイヤーからボス方向の向き取得
		Vector3 leng = Vector3(bossP - playerP).GetNormalizeNum();

		//長さから目標位置取得
		leng = playerP + leng;
		//設定した高さを目標にする
		leng.y = data_.warning.height;

		//座標適応
		boss_->world_.translate_ = Lerp(data_.warning.stPos, leng, t);
	}
}

void BossStump::UpdateATK()
{
	//向きに速度をかけて移動処理
	boss_->world_.translate_ += data_.stump.velo * data_.stump.spd * (float)DeltaTimer::deltaTime_;

	//着地時処理
	if (boss_->world_.translate_.y <= 0) {
		//座標を０に設定
		boss_->world_.translate_.y = 0;
		//着地エフェクト発生
		boss_->SpawnStumpEffect(boss_->world_.GetWorldTranslate());
		//状態をスタン状態へ
		behaviReq_ = Stiffness;
	}

}

void BossStump::UpdateStiffness()
{
	//カウント達成で処理
	if (currentCount_ >= parameters_.stiffnessSec) {
		//帰還処理へ
		behaviReq_ = Back;
	}
	else {
		//割合取得
		float t = currentCount_ / parameters_.stiffnessSec;
		//アニメーション再生をTで処理
		boss_->SetAnimeTime(true, t);
	}
}

void BossStump::UpdateBack()
{
	//カウント達成で処理
	if (currentCount_ >= parameters_.backSec) {
		//攻撃が終了したことを伝える
		boss_->isFinishedATK_ = true;
	}
	else {
		//割合取得
		float t = currentCount_ / parameters_.backSec;
		//アニメーション再生をTで処理
		boss_->SetAnimeTime(true, t);
	}
}
#pragma endregion



