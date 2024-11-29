#include "BossBackStep.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossBackStep::BossBackStep()
{
	SetParam2Tree("バックステップ");
	treeData_.SetValue("ステップあと攻撃という判断がでるHPの削れ具合",&percentage_);
	treeData_.SetValue("初速度", &spd_);
	treeData_.SetValue("重力", &gravity_);
	treeData_.SetValue("上方向の強さ", &height_);
	treeData_.SetValue("アニメーション速度倍率", &animeEndMultiply_);
}

void BossBackStep::InitAIMing()
{
	//とぶ方向を取得
	Vector3 velo = boss_->GetBoss2PlayerDirection() * -1;
	velo.y = 0;
	//ノーマライズ
	velo = velo.GetNormalizeNum();
	//上向きの強さ設定
	velo.y = height_;
	//初速度を与える
	velo *= spd_;

	//計算したものを与える
	velo_ = velo;

	//加速度のセット
	acce_.y = -gravity_;

	//プレイヤー方向を向く
	boss_->SetDirection2Player();

	//アニメーションを設定
	boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::BackStep], parameters_.aimingSec, animeEndMultiply_, false);

}


void BossBackStep::InitBack()
{

	//割合で攻撃を変更
	float dif = (float)boss_->HP_ / (float)boss_->maxHP_;

	if (dif < percentage_) {
		backBehavior_ = BackBehavior::ATK;
		//余裕のある場合
		//boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Roll], parameters_.backSec, parameters_.backSec, false);
		//プレイヤー方向に突っ込む
		boss_->SetNextATK(0);
	}
	else {
		backBehavior_ = BackBehavior::IDLE;
		//余裕のある場合
		//boss_->SetAnimation(boss_->animeName_[(int)Boss::Animation::Idle3], parameters_.backSec, parameters_.backSec, false);
		//弾をうつ
		boss_->SetNextATK(1);
	}

}

void BossBackStep::UpdateAIMing()
{
	//加速度を足す
	velo_ += acce_ * (float)DeltaTimer::deltaTime_;
	//速度を座標に足す
	boss_->world_.translate_ += velo_*(float)DeltaTimer::deltaTime_;

	//座標0以下で状態変化
	if (boss_->world_.translate_.y <= 0) {
		behaviReq_ = Back;
	}
}


void BossBackStep::UpdateBack()
{
	switch (backBehavior_)
	{
	case BossBackStep::BackBehavior::IDLE:

		
		if (currentCount_>=parameters_.backSec) {
			//攻撃が終了したことを伝える
			boss_->isFinishedATK_ = true;
		}

		break;
	case BossBackStep::BackBehavior::ATK:



		break;
	case BossBackStep::BackBehavior::CountBackBehavior:
		break;
	default:
		break;
	}
}




