#include "PlayerDown.h"
#include"Game/Player/Player.h"
#include"DeltaTimer/DeltaTimer.h"
#include"Game/Player/Behavior/BehaviorManager/PlayerBehaviorManager.h"

PlayerDown::PlayerDown(PlayerBehaviorManager* player)
{
	playerBehaviorM_ = player;
}

void PlayerDown::Initialize()
{

	//吹き飛び方向取得
	Vector3 v =playerBehaviorM_->GetPlayer()->GetP2BossVelo()*-1;
	v.y = 0;
	v.SetNormalize();

	v.y = height_;

	velo_ = v * spd_;
	
}

void PlayerDown::Update()
{
	velo_.y -= gravity_ * (float)DeltaTimer::deltaTime_;

	playerBehaviorM_->GetPlayer()->world_.translate_ += velo_*(float)DeltaTimer::deltaTime_;

	if (playerBehaviorM_->GetPlayer()->world_.translate_.y <= 0) {
		playerBehaviorM_->GetPlayer()->world_.translate_.y = 0;

		playerBehaviorM_->GetPlayer()->behaviorReq_ = Player::State::Move;
	}
}
