#include "PlayerDown.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include<numbers>

PlayerDown::PlayerDown(Player* player)
{
	player_ = player;

	tree_.name_ = "down";
	tree_.SetValue("吹き飛ぶ角度の高さ", &height_);
	tree_.SetValue("とぶ初速度", &spd_);
	tree_.SetValue("重力", &gravity_);
	tree_.SetValue("アニメーションが変わりきるまでの速度", &changeAnimeSec_);
	tree_.SetValue("アニメーションループの時間", &loopSec_);

	tree_.SetValue("復帰時間", &randSec_);

}

void PlayerDown::Initialize()
{
	behavior_ = Behavior::Flying;

	//吹き飛び方向取得
	Vector3 v =player_->GetP2BossVelo()*-1;
	v.y = 0;
	v.SetNormalize();

	v.y = height_;

	velo_ = v * spd_;

	//ボス方向を見る
	Vector3 direc = player_->GetP2BossVelo();
	direc.y = 0;
	if (direc != Vector3(0, 0, 0)) {
		player_->world_.rotate_.y = GetYRotate({ direc.x,direc.z }) + ((float)std::numbers::pi);
	}

	player_->SetAnimation(player_->animeName_[(int)Player::AnimationData::Dawn], changeAnimeSec_, loopSec_, false);

	currentLand_ = 0.0f;
}

void PlayerDown::Update()
{
	if (behavior_ == Behavior::Flying) {
		//重力加算
		velo_.y -= gravity_ * (float)DeltaTimer::deltaTime_;

		player_->data_.velo_ = velo_;

		//もしYが0以下
		if (player_->world_.translate_.y < 0) {
			player_->world_.translate_.y = 0;
			player_->data_.velo_.SetZero();
			behavior_ = Behavior::landing;
			player_->SetAnimation(player_->animeName_[(int)Player::AnimationData::DawnBack], 0, returnAnimeSec_, false);

		}
	}
	else if(behavior_ == Behavior::landing){
		currentLand_ += (float)DeltaTimer::deltaTime_;

		if (currentLand_ >= randSec_) {
			player_->SetBehaviorReq (Player::Behavior::IDLE);
		}
		else {
			player_->Move(false,0.5f);

			float t = currentLand_ / randSec_;
			player_->SetAnimeTime(true, t);

		}
	}
}
