#include "PlayerDown.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"

PlayerDown::PlayerDown(Player* player)
{
	player_ = player;
}

void PlayerDown::Initialize()
{

	//吹き飛び方向取得
	Vector3 v =player_->GetP2BossVelo()*-1;
	v.y = 0;
	v.SetNormalize();

	v.y = height_;

	velo_ = v * spd_;
	
}

void PlayerDown::Update()
{
	velo_.y -= gravity_ * (float)DeltaTimer::deltaTime_;

	player_->world_.translate_ += velo_*(float)DeltaTimer::deltaTime_;

	if (player_->world_.translate_.y <= 0) {
		player_->world_.translate_.y = 0;

		player_->behaviorReq_ = Player::State::Move;
	}
}
