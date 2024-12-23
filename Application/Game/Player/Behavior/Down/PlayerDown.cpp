#include "PlayerDown.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include<numbers>

PlayerDown::PlayerDown(Player* player)
{
	//プレイヤーポインタ設定
	player_ = player;

	//デバッグようにツリーにデータ追加
	tree_.name_ = "down";
	tree_.SetValue("吹き飛ぶ角度の高さ", &height_);
	tree_.SetValue("とぶ初速度", &spd_);
	tree_.SetValue("重力", &gravity_);
	tree_.SetValue("アニメーションが変わりきるまでの速度", &changeAnimeSec_);
	tree_.SetValue("アニメーションループの時間", &loopSec_);
	tree_.SetValue("復帰時間", &randSec_);
	tree_.SetValue("復帰時移動速度倍率", &moveSpdMuli_);
}

void PlayerDown::Initialize()
{
	//状態を飛行状態に設定
	behavior_ = Behavior::Flying;

	//吹き飛び方向取得
	Vector3 v = player_->GetP2BossVelo() * -1;
	v.y = 0;
	v.SetNormalize();

	//高さ設定
	v.y = height_;

	//速度分かける
	velo_ = v * spd_;

	//ボス方向を見る
	Vector3 direc = player_->GetP2BossVelo();
	direc.y = 0;
	if (direc != Vector3(0, 0, 0)) {
		player_->world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}

	//アニメーションを設定
	player_->animeManager_->SetAnimation(PAnimeM::Dawn, changeAnimeSec_, loopSec_, false);

	//カウントリセット
	currentLand_ = 0.0f;
}

void PlayerDown::Update()
{
	//吹っ飛んでいる状態の時
	if (behavior_ == Behavior::Flying) {
		//重力加算
		velo_.y -= gravity_ * (float)DeltaTimer::deltaTime_;
		//速度をパラメータに追加
		player_->data_.velo_ = velo_;

		//もしYが0以下
		if (player_->world_.translate_.y < 0) {
			//高さを0に合わせる
			player_->world_.translate_.y = 0;
			//速度初期化
			player_->data_.velo_.SetZero();

			//体力が尽きた場合
			if (player_->data_.currentHP >= player_->data_.nowHP) {
				//死亡フラグON
				player_->data_.isDead = true;
				//速度初期化
				player_->data_.velo_.SetZero();
			}
			else {
				//尽きていない場合
				//状態を起き上がりに変更
				behavior_ = Behavior::landing;
				//アニメーションを立ち上がる物に変更
				player_->animeManager_->SetAnimation(PAnimeM::DawnBack, 0, returnAnimeSec_, false);
			}
		}
	}//起き上がり処理
	else if (behavior_ == Behavior::landing) {
		//カウント追加
		currentLand_ += (float)DeltaTimer::deltaTime_;

		//カウントが満了したとき
		if (currentLand_ >= randSec_) {
			//状態を変更
			player_->SetBehaviorReq(Player::Behavior::IDLE);
		}
		else {
			//少しゆっくりに移動可能
			player_->Move(false, moveSpdMuli_);

			//割合取得
			float t = currentLand_ / randSec_;
			//アニメーション再生進度変更
			player_->animeManager_->SetAnimeTime(true, t);
		}
	}
}
