#include "BossBackStep.h"
#include"Game/Boss/Boss.h"
#include"DeltaTimer/DeltaTimer.h"

BossBackStep::BossBackStep()
{
	//デバッグ用にツリーに値を設定
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
	float dif = (float)boss_->nowHP / (float)boss_->maxHP_;

	//割合が一定以下の場合
	if (dif < percentage_) {

		//再び攻撃状態へ
		backBehavior_ = BackBehavior::ATK;
		//余裕のない場合
		//プレイヤー方向に突っ込む
		boss_->SetNextATK(ATKNum::Stump);
	}
	else {//一定以上の場合

		//待機状態へ
		backBehavior_ = BackBehavior::IDLE;
		//余裕のある場合
		//弾をうつ
		boss_->SetNextATK(ATKNum::Shot);
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
		//状態を終了状態へ
		behaviReq_ = Back;
	}
}





