#include "PlayerHit.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"

PlayerHit::PlayerHit(Player* player)
{
	player_ = player;


	tree_.SetValue("速度減衰率", &multiplySpd_);
	tree_.SetValue("続く時間", &sec_);
}

void PlayerHit::Initialize()
{
	current_ = 0;
	player_->data_.velo_.SetZero();
}

void PlayerHit::Update()
{
	//移動だけおこなえるように
	player_->Move(true,multiplySpd_);

	//カウント進行
	current_ += (float)DeltaTimer::deltaTime_;

	//カウント終了で待機へ
	if (current_ <= sec_) {
		player_->SetBehaviorReq(Player::Behavior::IDLE);
	}
}
